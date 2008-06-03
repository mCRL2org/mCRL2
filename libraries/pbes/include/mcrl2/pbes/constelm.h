// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/constelm.h
/// \brief The constelm algorithm.

#ifndef MCRL2_PBES_CONSTELM_H
#define MCRL2_PBES_CONSTELM_H

#include <iostream>
#include <utility>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

  template <typename IdentifierGenerator, typename DataRewriter>
  class pbes_constelm_algorithm
  {
    protected:
      typedef std::map<data::data_variable, data::data_expression> constraint_map;

      // Represents an edge of the dependency graph. The assignments are stored
      // implicitly using the 'right' parameter. The condition determines under
      // what circumstances the influence of the edge is propagated to its target
      // vertex.
      struct edge
      {
        propositional_variable left;
        propositional_variable_instantiation right;
        data::data_expression condition;

        edge()
        {}

        edge(
          propositional_variable l,
          propositional_variable_instantiation r,
          data::data_expression c = data::data_expr::true_()
        )
         : left(l), right(r), condition(c)
        {}       

        // Returns a string representation of the edge.
        std::string to_string() const
        {
          std::ostringstream out;
          out << "(" << pp(left.name()) << ", " << pp(right.name()) << ")  label = " << pp(right) << "  condition = " << pp(condition);
          return out.str();
        }
      };

      // Represents a vertex of the dependency graph.
      struct vertex
      {
        propositional_variable variable;
        constraint_map constraints;
        std::set<data::data_variable> non_constants;

        vertex()
        {}

        vertex(propositional_variable v)
          : variable(v)
        {}

        // Returns true if the data expression is constant.
        // TODO: what to do with free variables?
        bool is_constant_expression(data::data_expression d) const
        {
          return data::find_all_data_variables(d).empty();
        }

        // Return true if the data variable v appears in constraints, and
        // not in non_constants.
        bool is_constant(data::data_variable v) const
        {
          return (constraints.find(v) != constraints.end() && non_constants.find(v) == non_constants.end());
        }

        // Returns the constant parameters of this vertex.
        std::vector<data::data_variable> constant_parameters() const
        {
          std::vector<data::data_variable> result;
          for (data::data_variable_list::iterator i = variable.parameters().begin(); i != variable.parameters().end(); ++i)
          {
            if (is_constant(*i))
            {
              result.push_back(*i);
            }
          }
          return result;
        }

        // Returns a string representation of the vertex.
        std::string to_string() const
        {
          std::ostringstream out;
          out << pp(variable) << "  assertions = ";
          for (constraint_map::const_iterator i = constraints.begin(); i != constraints.end(); ++i)
          {
            std::string lhs = pp(i->first);
            std::string rhs = is_constant(i->first) ? pp(i->second) : "NaC";
            out << "{" << lhs << " := " << rhs << "} ";
          }
          return out.str();
        }

        // Accept new values for the parameters, and update the constraints accordingly.
        void update(data::data_expression condition,
                    data::data_expression_list new_values,
                    DataRewriter r,
                    const constraint_map& new_value_constraints = constraint_map())
        {
//std::cout << "<updating> " << pp(variable) << " new_values = " << pp(new_values) << std::endl;
          data::data_variable_list params = variable.parameters();
          if (condition == data::data_expr::true_())
          {
            data::data_expression_list::iterator i;
            data::data_variable_list::iterator j;
            for (i = new_values.begin(), j = params.begin(); i != new_values.end(); ++i, ++j)
            {
              // handle the parameter d
              data::data_variable d = *j;
              constraint_map::iterator k = constraints.find(d);

              if (k != constraints.end())
              {
                if (non_constants.find(d) == non_constants.end())
                {
                  data::data_expression old_value = k->second;
                  data::data_expression new_value = r(data::data_variable_map_replace(*i, new_value_constraints));
//std::cout << "<old_value> " << pp(old_value) << " new_value " << pp(new_value) << std::endl;
                  if (old_value != new_value)
                  {
                    non_constants.insert(d);
                  }
                }
              }
              else
              {
                data::data_expression new_value = r(data::data_variable_map_replace(*i, new_value_constraints));
//std::cout << " new_value " << pp(new_value) << std::endl;
                constraints[d] = new_value;
                if (!is_constant_expression(new_value))
                {
//std::cout << pp(new_value) << " is not constant!" << std::endl;
                  non_constants.insert(d);
                }
              }
            }
          }
//std::cout << "<updated> " << to_string() << std::endl;
        }
      };

      typedef std::map<core::identifier_string, vertex> vertex_map;
      typedef std::map<core::identifier_string, std::vector<edge> > edge_map;

      // Creates names that are guaranteed to be unique.
      IdentifierGenerator m_generator;

      // Compares data expressions for equality.
      DataRewriter m_rewriter;

      // The vertices of the dependency graph. They are stored in a map, to
      // support searching for a vertex.
      vertex_map m_vertices;

      // The edges of the dependency graph. They are stored in a map, to
      // easily access all out-edges corresponding to a particular vertex.
      edge_map m_edges;

      // Prints the vertices of the dependency graph.
      void print_vertices() const
      {
        for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
        {
          std::cout << i->second.to_string() << std::endl;
        }
      }

      // Prints the edges of the dependency graph.
      void print_edges() const
      {
        for (typename edge_map::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
        {
          for (typename std::vector<edge>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
          {
            std::cout << j->to_string() << std::endl;
          }
        }
      }

    public:
      pbes_constelm_algorithm(IdentifierGenerator generator, DataRewriter rewriter)
        : m_generator(generator), m_rewriter(rewriter)
      {}

      template <typename Container>
      void run(pbes<Container>& p, pbes_expression kappa)
      {
        // compute the vertices and edges of the dependency graph
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          m_vertices[name] = vertex(i->variable());

          typename edge_map::iterator ei = m_edges.insert(std::make_pair(name, std::vector<edge>())).first;
          std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(i->formula());
          for (std::set<propositional_variable_instantiation>::iterator k = inst.begin(); k != inst.end(); ++k)
          {
            ei->second.push_back(edge(i->variable(), *k));
          }
        }

        // fill in the edge conditions of the dependency graph
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          const vertex& u = m_vertices[name];
          const std::vector<edge>& edges = m_edges[name];

          // TODO: build a tree with true- and false-conditions
          for (typename std::vector<edge>::const_iterator j = edges.begin(); j != edges.end(); ++j)
          {
            // TODO: use the tree to set the condition of edge *j
          }
        }

        // initialize the todo list of vertices that need to be processed
        std::deque<propositional_variable> todo;
        std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(kappa);
        for (std::set<propositional_variable_instantiation>::iterator i = inst.begin(); i != inst.end(); ++i)
        {
          data::data_expression_list new_values = i->parameters();
          vertex& u = m_vertices[i->name()];
          u.update(data::data_expr::true_(), new_values, m_rewriter);
          todo.push_back(u.variable);
        }

        if (mcrl2::core::gsDebug)
        {
          std::cout << "\n--- initial vertices ---" << std::endl;
          print_vertices();
          std::cout << "\n--- edges ---" << std::endl;
          print_edges();
        }

        while (!todo.empty())
        {
          propositional_variable var = todo.front();
          todo.pop_front();

          const vertex& u = m_vertices[var.name()];
          std::vector<edge>& u_edges = m_edges[var.name()];
          data::data_variable_list Xparams = u.variable.parameters();

          for (typename std::vector<edge>::const_iterator ei = u_edges.begin(); ei != u_edges.end(); ++ei)
          {
            const edge& e = *ei;
            vertex& v = m_vertices[e.right.name()];
            v.update(e.condition, e.right.parameters(), m_rewriter, u.constraints);
          }
        }

        if (mcrl2::core::gsDebug)
        {
          std::cout << "\n--- final vertices ---" << std::endl;
          print_vertices();
        }

        if (mcrl2::core::gsVerbose)
        {
          std::cout << "\nremoving the following constant parameters:" << std::endl;
          for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
          {
            const vertex& u = i->second;
            std::vector<data::data_variable> removed = u.constant_parameters();
            for (std::vector<data::data_variable>::iterator j = removed.begin(); j != removed.end(); ++j)
            {
              std::cout << "(" << pp(u.variable.name()) << ", " << pp(*j) << ")" << std::endl;
            }
          }
        }
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_CONSTELM_H
