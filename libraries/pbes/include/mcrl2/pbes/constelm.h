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

// #define MCRL2_PBES_CONSTELM_DEBUG

#include <sstream>
#include <iostream>
#include <utility>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/remove_parameters.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {
  inline
  bool operator<(propositional_variable_instantiation v, propositional_variable_instantiation w)
  {
    return ATermAppl(v) < ATermAppl(w);
  }

  struct true_false_pair
  {
    pbes_expression TC;
    pbes_expression FC;

    true_false_pair()
      : TC(pbes_expr::true_()), FC(pbes_expr::true_())
    {}
    
    true_false_pair(pbes_expression t, pbes_expression f)
      : TC(t), FC(f)
    {}
  };

  struct apply_exists
  {
    data::data_variable_list variables_;

    apply_exists(data::data_variable_list variables)
      : variables_(variables)
    {}

    void operator()(true_false_pair& p) const
    {
      using namespace pbes_expr_optimized;
      p.TC = exists(variables_, p.TC);
      p.FC = forall(variables_, p.FC);
    }
  };

  struct apply_forall
  {
    data::data_variable_list variables_;

    apply_forall(data::data_variable_list variables)
      : variables_(variables)
    {}

    void operator()(true_false_pair& p) const
    {
      using namespace pbes_expr_optimized;
      p.TC = forall(variables_, p.TC);
      p.FC = exists(variables_, p.FC);
    }
  };

  typedef std::multimap<propositional_variable_instantiation, std::vector<true_false_pair> > condition_map;

  struct edge_condition
  {
    pbes_expression TC;
    pbes_expression FC;
    condition_map condition;  // condT + condF

    true_false_pair TCFC() const
    {
      return true_false_pair(TC, FC);
    }

    pbes_expression compute_condition(const std::vector<true_false_pair>& c) const
    {
      using namespace pbes_expr_optimized;
      pbes_expression result = true_();      
      for (std::vector<true_false_pair>::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        result = and_(result, not_(i->TC));
        result = and_(result, not_(i->FC));
      }
      return result;
    }
  };

  struct edge_condition_visitor: public pbes_expression_visitor<pbes_expression, edge_condition>
  {
    // N.B. As a side effect ec1 and ec2 are changed!!!
    void merge_conditions(edge_condition& ec1,
                          edge_condition& ec2,
                          edge_condition& ec
                         )
    {
      for (condition_map::iterator i = ec1.condition.begin(); i != ec1.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        ec.condition.insert(*i);
      }
      for (condition_map::iterator i = ec2.condition.begin(); i != ec2.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        ec.condition.insert(*i);
      }
    }

    /// Visit data expression node.
    ///
    bool visit_data_expression(const pbes_expression& e, const data::data_expression& d, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      ec.TC = d;
      ec.FC = not_(d);
      return stop_recursion;
    }

    /// Visit true node.
    ///
    bool visit_true(const pbes_expression& e, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      ec.TC = true_();
      ec.FC = false_();
      return stop_recursion;
    }

    /// Visit false node.
    ///
    bool visit_false(const pbes_expression& e, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      ec.TC = false_();
      ec.FC = true_();
      return stop_recursion;
    }

    /// Visit not node.
    ///
    bool visit_not(const pbes_expression& e, const pbes_expression& arg, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      edge_condition ec_arg;
      visit(arg, ec_arg);
      ec.TC = ec_arg.FC;
      ec.FC = ec_arg.TC;
      ec.condition = ec_arg.condition;
      return stop_recursion;
    }

    /// Visit and node.
    ///
    bool visit_and(const pbes_expression& e, const pbes_expression& left, const pbes_expression&  right, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = and_(ec_left.TC, ec_right.TC);
      ec.FC = or_(ec_left.FC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return stop_recursion;
    }

    /// Visit or node.
    ///
    bool visit_or(const pbes_expression& e, const pbes_expression&  left, const pbes_expression&  right, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = or_(ec_left.TC, ec_right.TC);
      ec.FC = and_(ec_left.FC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return stop_recursion;
    }

    /// Visit imp node.
    ///
    bool visit_imp(const pbes_expression& e, const pbes_expression&  left, const pbes_expression&  right, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = or_(ec_left.FC, ec_right.TC);
      ec.FC = and_(ec_left.TC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return stop_recursion;
    }

    /// Visit forall node.
    ///
    bool visit_forall(const pbes_expression& e, const data::data_variable_list&  variables, const pbes_expression& expr, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      visit(expr, ec);
      for (condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        std::for_each(i->second.begin(), i->second.end(), apply_forall(variables));
      }
      return stop_recursion;
    }

    /// Visit exists node.
    ///
    bool visit_exists(const pbes_expression& e, const data::data_variable_list&  variables, const pbes_expression& expr, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      visit(expr, ec);
      for (condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        std::for_each(i->second.begin(), i->second.end(), apply_exists(variables));
      }
      return stop_recursion;
    }

    /// Visit propositional variable node.
    ///
    bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& v, edge_condition& ec)
    {
      using namespace pbes_expr_optimized;
      ec.TC = false_();
      ec.FC = false_();
      std::vector<true_false_pair> c;
      c.push_back(true_false_pair(false_(), false_()));
      ec.condition.insert(std::make_pair(v, c));
      return stop_recursion;
    }
  };

} // namespace detail

  template <typename DataRewriter, typename PbesRewriter>
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
        pbes_expression condition;

        edge()
        {}

        edge(
          propositional_variable l,
          propositional_variable_instantiation r,
          pbes_expression c = pbes_expr::true_()
        )
         : left(l), right(r), condition(c)
        {}

        // Returns a string representation of the edge.
        std::string to_string() const
        {
          std::ostringstream out;
          out << "(" << mcrl2::core::pp(left.name()) << ", " << mcrl2::core::pp(right.name()) << ")  label = " << mcrl2::core::pp(right) << "  condition = " << mcrl2::core::pp(condition);
          return out.str();
        }
      };

      // Represents a vertex of the dependency graph.
      struct vertex
      {
        propositional_variable variable;
        
        // Maps data variables to data expressions. If the right hand side is a data
        // variable, it means that it represents NaC ("not a constant"). In such case,
        // a fresh variable is chosen, that does not appear anywhere else in the pbes
        // that is under consideration.
        constraint_map constraints;

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

        // Return true if the data variable v has been assigned a constant expression.
        bool is_constant(data::data_variable v) const
        {
          constraint_map::const_iterator i = constraints.find(v);
          return i != constraints.end() && !data::is_data_variable(i->second);
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

        // Returns the indices of the constant parameters of this vertex.
        std::vector<int> constant_parameter_indices() const
        {
          std::vector<int> result;
          int index = 0;
          for (data::data_variable_list::iterator i = variable.parameters().begin(); i != variable.parameters().end(); ++i, index++)
          {
            if (is_constant(*i))
            {
              result.push_back(index);
            }
          }
          return result;
        }

        // Returns a string representation of the vertex.
        std::string to_string() const
        {
          std::ostringstream out;
          out << mcrl2::core::pp(variable) << "  assertions = ";
          for (constraint_map::const_iterator i = constraints.begin(); i != constraints.end(); ++i)
          {
            std::string lhs = mcrl2::core::pp(i->first);
            std::string rhs = is_constant(i->first) ? mcrl2::core::pp(i->second) : "NaC";
            out << "{" << lhs << " := " << rhs << "} ";
          }
          return out.str();
        }

        // Assign new values to the parameters of this vertex, and update the constraints accordingly.
        // The new values have a number of constraints.
        template <typename IdentifierGenerator>
        bool update(data::data_expression_list new_values,
                    const constraint_map& new_value_constraints,
                    DataRewriter datar,
                    IdentifierGenerator name_generator
                   )
        {
          bool changed = false;
          data::data_variable_list params = variable.parameters();
          data::data_expression_list::iterator i;
          data::data_variable_list::iterator j;
          for (i = new_values.begin(), j = params.begin(); i != new_values.end(); ++i, ++j)
          {
            // handle the parameter d
            data::data_variable d = *j;
            constraint_map::iterator k = constraints.find(d);

            if (k != constraints.end())
            {
              if (!data::is_data_variable(k->second)) // d has been assigned a constant value
              {
                data::data_expression old_value = k->second;
                data::data_expression new_value = datar(data::data_variable_map_replace(*i, new_value_constraints));
                if (old_value != new_value)
                {
                  // mark the parameter as NaC by assigning a fresh variable to it
                  k->second = data::data_variable(name_generator(), j->sort());
                  changed = true;
                }
              }
            }
            else
            {
              changed = true;
              data::data_expression new_value = datar(data::data_variable_map_replace(*i, new_value_constraints));
              if (is_constant_expression(new_value))
              {
                constraints[d] = new_value;
              }
              else
              {
                // mark the parameter as NaC by assigning a fresh variable to it
                constraints[d] = data::data_variable(name_generator(), j->sort());
              }
            }
          }
          
          return changed;
        }

        // Removes NaC constraints.
        void remove_nac_constraints()
        {
          // See [Josuttis, "The C++ Standard Library" page 205]
          constraint_map::iterator pos;
          for (pos = constraints.begin(); pos != constraints.end(); )
          {
            if (data::is_data_variable(pos->second)) // the value is NaC
            {
              constraints.erase(pos++);
            }
            else
            {
              ++pos;
            }
          }
        }
      };

      typedef std::map<core::identifier_string, vertex> vertex_map;
      typedef std::map<core::identifier_string, std::vector<edge> > edge_map;

      // Compares data expressions for equality.
      DataRewriter m_data_rewriter;

      // Compares data expressions for equality.
      PbesRewriter m_pbes_rewriter;

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
      pbes_constelm_algorithm(DataRewriter datar, PbesRewriter pbesr)
        : m_data_rewriter(datar), m_pbes_rewriter(pbesr)
      {}

      template <typename Container, typename IdentifierGenerator>
      void run(pbes<Container>& p, pbes_expression kappa, IdentifierGenerator name_generator, bool compute_conditions = false)
      {
        using detail::edge_condition;
        using detail::edge_condition_visitor;
        using detail::condition_map;
        using namespace pbes_expr_optimized;
        
        // compute the vertices and edges of the dependency graph
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          m_vertices[name] = vertex(i->variable());

          if (compute_conditions)
          {
            // use an edge_condition_visitor to compute the edges
            edge_condition ec;
            edge_condition_visitor visitor;
            visitor.visit(i->formula(), ec);
            if (!ec.condition.empty())
            {
              std::vector<edge>& edges = m_edges[name];
              for (condition_map::iterator j = ec.condition.begin(); j != ec.condition.end(); ++j)
              {
                propositional_variable_instantiation X = j->first;
                pbes_expression condition = ec.compute_condition(j->second);
                edges.push_back(edge(i->variable(), X, condition));
              }
            }
          }
          else
          {
            // use find function to compute the edges
            std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(i->formula());
            if (!inst.empty())
            {
              std::vector<edge>& edges = m_edges[name];
              for (std::set<propositional_variable_instantiation>::iterator k = inst.begin(); k != inst.end(); ++k)
              {
                edges.push_back(edge(i->variable(), *k));
              }
            }
          }
        }

        // initialize the todo list of vertices that need to be processed
        std::deque<propositional_variable> todo;
        std::set<propositional_variable_instantiation> inst = find_all_propositional_variable_instantiations(kappa);
        for (std::set<propositional_variable_instantiation>::iterator i = inst.begin(); i != inst.end(); ++i)
        {
          data::data_expression_list new_values = i->parameters();
          vertex& u = m_vertices[i->name()];
          u.update(new_values, constraint_map(), m_data_rewriter, name_generator);
          todo.push_back(u.variable);
        }

        if (mcrl2::core::gsDebug)
        {
          std::cout << "\n--- initial vertices ---" << std::endl;
          print_vertices();
          std::cout << "\n--- edges ---" << std::endl;
          print_edges();
        }

        // propagate constraints over the edges until the todo list is empty
        while (!todo.empty())
        {
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cout << "\n<todo list>" << core::pp(propositional_variable_list(todo.begin(), todo.end())) << std::endl;
#endif
          propositional_variable var = todo.front();

          // remove all occurrences of var from todo
          todo.erase(std::remove(todo.begin(), todo.end(), var), todo.end());

          const vertex& u = m_vertices[var.name()];
          std::vector<edge>& u_edges = m_edges[var.name()];
          data::data_variable_list Xparams = u.variable.parameters();

          for (typename std::vector<edge>::const_iterator ei = u_edges.begin(); ei != u_edges.end(); ++ei)
          {
            const edge& e = *ei;
            vertex& v = m_vertices[e.right.name()];
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cout << "\n<updating edge>" << e.to_string() << std::endl;
std::cout << "  <source vertex       >" << u.to_string() << std::endl;
std::cout << "  <target vertex before>" << v.to_string() << std::endl;
#endif

            pbes_expression value = m_pbes_rewriter(data::data_variable_map_replace(e.condition, u.constraints));
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cout << "\nEvaluated condition " << core::pp(data::data_variable_map_replace(e.condition, u.constraints)) << " to " << core::pp(value) << std::endl;
#endif
            if (value != true_() && value != false_())
            {
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cout << "\nCould not evaluate condition " << core::pp(data::data_variable_map_replace(e.condition, u.constraints)) << " to true or false";
#endif
            }
            if (value != false_())
            {              
              bool changed = v.update(e.right.parameters(), u.constraints, m_data_rewriter, name_generator);
              if (changed)
              {
                todo.push_back(v.variable);
              }
            }
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cout << "  <target vertex after >" << v.to_string() << std::endl;
#endif
          }
        }

        if (mcrl2::core::gsDebug)
        {
          std::cout << "\n--- final vertices ---" << std::endl;
          print_vertices();
        }

        // print the parameters that will be removed
        if (mcrl2::core::gsVerbose)
        {
          std::cout << "\nremoving the following constant parameters and equations:" << std::endl;
          for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
          {
            const vertex& u = i->second;
            if (u.constraints.empty())
            {
              std::cout << "  equation " << core::pp(u.variable) << std::endl;
            }
            else
            { 
              std::vector<data::data_variable> removed = u.constant_parameters();
              for (std::vector<data::data_variable>::iterator j = removed.begin(); j != removed.end(); ++j)
              {
                std::cout << "  parameter (" << mcrl2::core::pp(u.variable.name()) << ", " << core::pp(*j) << ")" << std::endl;
              }
            }
          }
        }

        // remove the constant parameters
        std::map<core::identifier_string, std::vector<int> > to_be_removed_variables;
        for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          vertex& v = m_vertices[name];
          std::vector<int> r = v.constant_parameter_indices();
          if (!r.empty())
          {
            to_be_removed_variables[name] = r;
          }
        }
        remove_parameters(p, to_be_removed_variables);

        // Apply the constraints to the equations.
        // TODO: The equations without constraints may be removed, but then the references
        // to the propositional variables of this equations should be removed as well.
        for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          vertex& v = m_vertices[name];
          
          // do not apply NaC constraints
          v.remove_nac_constraints();

          if (!v.constraints.empty())
          {
            *i = pbes_equation(
              i->symbol(),
              i->variable(),
              data::data_variable_map_replace(i->formula(), v.constraints)
            );
          }
        }
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_CONSTELM_H
