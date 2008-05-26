// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/constelm.h
/// \brief The constelm algorithm.

#ifndef MCRL2_PBES_PARELM_H
#define MCRL2_PBES_PARELM_H

#include <utility>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

class pbes_constelm_algorithm
{
  protected:
    // Represents an edge of the dependency graph. The assignments are stored
    // implicitly using the 'right' parameter. The condition determines under
    // what circumstances the influence of the edge is propagated to its target
    // vertex.
    struct edge
    {
      propositional_variable left;
      propositional_variable_instantiation right;
      data::data_expression condition;

      edge(
        propositional_variable l,
        propositional_variable_instantiation r,
        data::data_expression c = data::data_expr::true_()
      )
       : left(l), right(r), condition(c)
      {}
    };

    struct vertex
    {
      propositional_variable variable;
      std::map<data::data_variable, data::data_expression> assertions;
        
      vertex(propositional_variable v)
        : variable(v)
      {}
    };

    // The vertices of the dependency graph. They are stored in a map, to
    // support searching for a vertex.
    std::map<core::identifier_string, vertex> m_vertices;

    // The edges of the dependency graph. They are stored in a map, to
    // easily access all out-edges corresponding to a particular vertex.
    std::map<core::identifier_string, std::vector<edge> > m_edges;

  public:
    template <typename Container>
    void run(pbes<Container>& p, pbes_expression kappa) const
    {
      // compute the vertices and edges of the dependency graph
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        core::identifier_string name = i->variable().name();
        m_vertices[name] = vertex(i->variable());

        std::map<core::identifier_string, std::vector<edge> >::iterator j = m_edges.insert(std::make_pair(name, std::vector<edge>()));
        std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(i->formula());
        for (std::set<propositional_variable_instantiation>::k = inst.begin(); k != inst.end(); ++k)
        {
          (i->second).push_back(edge(i->variable(), *k));
        }
      }

      // fill in the edge conditions of the dependency graph
      for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        core::identifier_string name = i->variable().name();
        const vertex& u = m_vertices[name];
        const std::vector<edge>& edges = m_edges[name];

        // TODO: build a tree with true- and false-conditions

        for (std::vector<edge>::const_iterator j = edges.begin(); j != edges.end(); ++j)
        {
          // TODO: use the tree to set the condition of edge *j
        }
      }

      // initialize the todo list of vertices that need to be processed
      std::deque<propositional_variable> todo;
      std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(kappa);
      for (std::set<propositional_variable_instantiation>::i = inst.begin(); i != inst.end(); ++i)
      {
        propositional_variable v = m_vertices[i->name()].variable;
        todo.push_back(v);
        m_edges.push_back(edge(v, *j));
      }
      
      while (!todo.empty())
      {
        propositional_variable var = todo.front();
        core::identifier_string name = var.name();
        todo.pop_front();
        std::vector<edge>& edges = m_edges[name];
        for (std::vector<edge>::const_iterator i = edges.begin(); i != edges.end(); ++i)
        {
          const edge& e = m_edges[name];
          const vertex& u = m_vertices[name];
          const vertex& v = m_vertices[e.right.name()];
          // TODO: propagate the assertions from u to v
          // if (e.condition evaluates to true for u.assertions)
          // {
          //   compute v.assertions
          //   if v.assertions have changed
          //   {
          //     todo.push_back(v.left)
          //   }
          // }
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARELM_H
