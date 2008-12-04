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
  bool less_term(atermpp::aterm_appl v, atermpp::aterm_appl w)
  {
    return ATermAppl(v) < ATermAppl(w);
  }

  template <typename Term>
  struct true_false_pair
  {
    typedef typename core::term_traits<Term>::term_type term_type;
    typedef typename core::term_traits<Term> tr;

    term_type TC;
    term_type FC;

    true_false_pair()
      : TC(tr::true_()), FC(tr::true_())
    {}

    true_false_pair(term_type t, term_type f)
      : TC(t), FC(f)
    {}
  };

  template <typename Term>
  struct apply_exists
  {
    typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<Term> tr;

    variable_sequence_type variables_;

    apply_exists(variable_sequence_type variables)
      : variables_(variables)
    {}

    void operator()(true_false_pair<Term>& p) const
    {
      p.TC = tr::exists(variables_, p.TC);
      p.FC = tr::forall(variables_, p.FC);
    }
  };

  template <typename Term>
  struct apply_forall
  {
    typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<Term> tr;

    variable_sequence_type variables_;

    apply_forall(variable_sequence_type variables)
      : variables_(variables)
    {}

    void operator()(true_false_pair<Term>& p) const
    {
      p.TC = tr::forall(variables_, p.TC);
      p.FC = tr::exists(variables_, p.FC);
    }
  };

  template <typename Term>
  struct constelm_edge_condition
  {
    typedef typename core::term_traits<Term>::term_type term_type;
    typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;
    typedef typename core::term_traits<Term> tr;
    typedef std::multimap<propositional_variable_type, std::vector<true_false_pair<Term> > > condition_map;

    term_type TC;
    term_type FC;
    condition_map condition;  // condT + condF

    true_false_pair<Term> TCFC() const
    {
      return true_false_pair<Term>(TC, FC);
    }

    term_type compute_condition(const std::vector<true_false_pair<Term> >& c) const
    {
      term_type result = tr::true_();
      for (typename std::vector<true_false_pair<Term> >::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        result = core::optimized_and(result, core::optimized_not(i->TC));
        result = core::optimized_and(result, core::optimized_not(i->FC));
      }
      return result;
    }
  };

  template <typename Term>
  struct edge_condition_visitor: public pbes_expression_visitor<Term, constelm_edge_condition<Term> >
  {
    typedef typename core::term_traits<Term>::term_type term_type;
    typedef typename core::term_traits<Term>::variable_type variable_type;
    typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<Term>::data_term_type data_term_type;
    typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;
    typedef constelm_edge_condition<Term> edge_condition;
    typedef typename edge_condition::condition_map condition_map;
    typedef typename core::term_traits<Term> tr;

    // N.B. As a side effect ec1 and ec2 are changed!!!
    void merge_conditions(edge_condition& ec1,
                          edge_condition& ec2,
                          edge_condition& ec
                         )
    {
      for (typename condition_map::iterator i = ec1.condition.begin(); i != ec1.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        ec.condition.insert(*i);
      }
      for (typename condition_map::iterator i = ec2.condition.begin(); i != ec2.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        ec.condition.insert(*i);
      }
    }

    /// \brief Visit data expression node.
    bool visit_data_expression(const term_type& e, const data_term_type& d, edge_condition& ec)
    {
      ec.TC = d;
      ec.FC = core::optimized_not(d);
      return this->stop_recursion;
    }

    /// \brief Visit true node.
    bool visit_true(const term_type& e, edge_condition& ec)
    {
      ec.TC = tr::true_();
      ec.FC = tr::false_();
      return this->stop_recursion;
    }

    /// \brief Visit false node.
    bool visit_false(const term_type& e, edge_condition& ec)
    {
      ec.TC = tr::false_();
      ec.FC = tr::true_();
      return this->stop_recursion;
    }

    /// \brief Visit not node.
    bool visit_not(const term_type& e, const term_type& arg, edge_condition& ec)
    {
      edge_condition ec_arg;
      visit(arg, ec_arg);
      ec.TC = ec_arg.FC;
      ec.FC = ec_arg.TC;
      ec.condition = ec_arg.condition;
      return this->stop_recursion;
    }

    /// \brief Visit and node.
    bool visit_and(const term_type& e, const term_type& left, const term_type&  right, edge_condition& ec)
    {
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = core::optimized_and(ec_left.TC, ec_right.TC);
      ec.FC = core::optimized_or(ec_left.FC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return this->stop_recursion;
    }

    /// \brief Visit or node.
    bool visit_or(const term_type& e, const term_type&  left, const term_type&  right, edge_condition& ec)
    {
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = core::optimized_or(ec_left.TC, ec_right.TC);
      ec.FC = core::optimized_and(ec_left.FC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return this->stop_recursion;
    }

    /// \brief Visit imp node.
    bool visit_imp(const term_type& e, const term_type&  left, const term_type&  right, edge_condition& ec)
    {
      edge_condition ec_left;
      visit(left, ec_left);
      edge_condition ec_right;
      visit(right, ec_right);
      ec.TC = core::optimized_or(ec_left.FC, ec_right.TC);
      ec.FC = core::optimized_and(ec_left.TC, ec_right.FC);
      merge_conditions(ec_left, ec_right, ec);
      return this->stop_recursion;
    }

    /// \brief Visit forall node.
    bool visit_forall(const term_type& e, const variable_sequence_type& variables, const term_type& expr, edge_condition& ec)
    {
      visit(expr, ec);
      for (typename condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        std::for_each(i->second.begin(), i->second.end(), apply_forall<Term>(variables));
      }
      return this->stop_recursion;
    }

    /// \brief Visit exists node.
    bool visit_exists(const term_type& e, const variable_sequence_type&  variables, const term_type& expr, edge_condition& ec)
    {
      visit(expr, ec);
      for (typename condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
      {
        i->second.push_back(ec.TCFC());
        std::for_each(i->second.begin(), i->second.end(), apply_exists<Term>(variables));
      }
      return this->stop_recursion;
    }

    /// \brief Visit propositional variable node.
    bool visit_propositional_variable(const term_type& e, const propositional_variable_type& v, edge_condition& ec)
    {
      ec.TC = tr::false_();
      ec.FC = tr::false_();
      std::vector<true_false_pair<Term> > c;
      c.push_back(true_false_pair<Term>(tr::false_(), tr::false_()));
      ec.condition.insert(std::make_pair(v, c));
      return this->stop_recursion;
    }
  };

} // namespace detail

  template <typename Term, typename DataRewriter, typename PbesRewriter>
  class pbes_constelm_algorithm
  {
    public:
      typedef typename core::term_traits<Term>::term_type term_type;
      typedef typename core::term_traits<Term>::variable_type variable_type;
      typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
      typedef typename core::term_traits<Term>::data_term_type data_term_type;
      typedef typename core::term_traits<Term>::data_term_sequence_type data_term_sequence_type;
      typedef typename core::term_traits<Term>::string_type string_type;
      typedef typename core::term_traits<Term>::propositional_variable_decl_type propositional_variable_decl_type;
      typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;
      typedef typename detail::edge_condition_visitor<Term> edge_condition_visitor;
      typedef typename edge_condition_visitor::edge_condition edge_condition;
      typedef typename edge_condition_visitor::condition_map condition_map;
      typedef typename core::term_traits<Term> tr;

    protected:
      typedef std::map<variable_type, data_term_type> constraint_map;

      // Represents an edge of the dependency graph. The assignments are stored
      // implicitly using the 'right' parameter. The condition determines under
      // what circumstances the influence of the edge is propagated to its target
      // vertex.
      struct edge
      {
        propositional_variable_decl_type left;
        propositional_variable_type right;
        term_type condition;

        edge()
        {}

        edge(
          propositional_variable_decl_type l,
          propositional_variable_type r,
          term_type c = pbes_expr::true_()
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
        propositional_variable_decl_type variable;

        // Maps data variables to data expressions. If the right hand side is a data
        // variable, it means that it represents NaC ("not a constant"). In such case,
        // a fresh variable is chosen, that does not appear anywhere else in the pbes
        // that is under consideration.
        constraint_map constraints;

        vertex()
        {}

        vertex(propositional_variable_decl_type v)
          : variable(v)
        {}

        // Return true if the data variable v has been assigned a constant expression.
        bool is_constant(variable_type v) const
        {
          typename constraint_map::const_iterator i = constraints.find(v);
          return i != constraints.end() && !data::is_data_variable(i->second);
        }

        // Returns the constant parameters of this vertex.
        std::vector<variable_type> constant_parameters() const
        {
          std::vector<variable_type> result;
          for (typename variable_sequence_type::iterator i = variable.parameters().begin(); i != variable.parameters().end(); ++i)
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
          for (typename variable_sequence_type::iterator i = variable.parameters().begin(); i != variable.parameters().end(); ++i, index++)
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
          for (typename constraint_map::const_iterator i = constraints.begin(); i != constraints.end(); ++i)
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
        bool update(data_term_sequence_type new_values,
                    const constraint_map& new_value_constraints,
                    DataRewriter datar,
                    IdentifierGenerator name_generator
                   )
        {
          bool changed = false;
          variable_sequence_type params = variable.parameters();
          typename data_term_sequence_type::iterator i;
          typename variable_sequence_type::iterator j;
          for (i = new_values.begin(), j = params.begin(); i != new_values.end(); ++i, ++j)
          {
            // handle the parameter d
            variable_type d = *j;
            typename constraint_map::iterator k = constraints.find(d);

            if (k != constraints.end())
            {
              if (!data::is_data_variable(k->second)) // d has been assigned a constant value
              {
                data_term_type old_value = k->second;
                data_term_type new_value = datar(data::data_variable_map_replace(*i, new_value_constraints));
                if (old_value != new_value)
                {
                  // mark the parameter as NaC by assigning a fresh variable to it
                  k->second = variable_type(name_generator(), j->sort());
                  changed = true;
                }
              }
            }
            else
            {
              changed = true;
              data_term_type new_value = datar(data::data_variable_map_replace(*i, new_value_constraints));
              if (core::term_traits<data_term_type>::is_constant(new_value))
              {
                constraints[d] = new_value;
              }
              else
              {
                // mark the parameter as NaC by assigning a fresh variable to it
                constraints[d] = variable_type(name_generator(), j->sort());
              }
            }
          }

          return changed;
        }

        // Removes NaC constraints.
        void remove_nac_constraints()
        {
          // See [Josuttis, "The C++ Standard Library" page 205]
          typename constraint_map::iterator pos;
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

      typedef std::map<string_type, vertex> vertex_map;
      typedef std::map<string_type, std::vector<edge> > edge_map;

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

      // Store the removed variables.
      std::map<propositional_variable_decl_type, std::set<variable_type> > m_removed;

      // Prints the vertices of the dependency graph.
      void print_vertices() const
      {
        for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
        {
          std::cerr << i->second.to_string() << std::endl;
        }
      }

      // Prints the edges of the dependency graph.
      void print_edges() const
      {
        for (typename edge_map::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
        {
          for (typename std::vector<edge>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
          {
            std::cerr << j->to_string() << std::endl;
          }
        }
      }

    public:
      pbes_constelm_algorithm(DataRewriter datar, PbesRewriter pbesr)
        : m_data_rewriter(datar), m_pbes_rewriter(pbesr)
      {}

      template <typename Container, typename IdentifierGenerator>
      void run(pbes<Container>& p, term_type kappa, IdentifierGenerator name_generator, bool compute_conditions = false)
      {
        // compute the vertices and edges of the dependency graph
        for (typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
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
              for (typename condition_map::iterator j = ec.condition.begin(); j != ec.condition.end(); ++j)
              {
                propositional_variable_type X = j->first;
                term_type condition = ec.compute_condition(j->second);
                edges.push_back(edge(i->variable(), X, condition));
              }
            }
          }
          else
          {
            // use find function to compute the edges
            std::set<propositional_variable_type> inst = find_all_propositional_variable_instantiations(i->formula());
            if (!inst.empty())
            {
              std::vector<edge>& edges = m_edges[name];
              for (typename std::set<propositional_variable_type>::iterator k = inst.begin(); k != inst.end(); ++k)
              {
                edges.push_back(edge(i->variable(), *k));
              }
            }
          }
        }

        // initialize the todo list of vertices that need to be processed
        std::deque<propositional_variable_decl_type> todo;
        std::set<propositional_variable_type> inst = find_all_propositional_variable_instantiations(kappa);
        for (typename std::set<propositional_variable_type>::iterator i = inst.begin(); i != inst.end(); ++i)
        {
          data_term_sequence_type new_values = i->parameters();
          vertex& u = m_vertices[i->name()];
          u.update(new_values, constraint_map(), m_data_rewriter, name_generator);
          todo.push_back(u.variable);
        }

        if (mcrl2::core::gsDebug)
        {
          std::cerr << "\n--- initial vertices ---" << std::endl;
          print_vertices();
          std::cerr << "\n--- edges ---" << std::endl;
          print_edges();
        }

        // propagate constraints over the edges until the todo list is empty
        while (!todo.empty())
        {
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cerr << "\n<todo list>" << core::pp(propositional_variable_list(todo.begin(), todo.end())) << std::endl;
#endif
          propositional_variable_decl_type var = todo.front();

          // remove all occurrences of var from todo
          todo.erase(std::remove(todo.begin(), todo.end(), var), todo.end());

          const vertex& u = m_vertices[var.name()];
          std::vector<edge>& u_edges = m_edges[var.name()];
          variable_sequence_type Xparams = u.variable.parameters();

          for (typename std::vector<edge>::const_iterator ei = u_edges.begin(); ei != u_edges.end(); ++ei)
          {
            const edge& e = *ei;
            vertex& v = m_vertices[e.right.name()];
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cerr << "\n<updating edge>" << e.to_string() << std::endl;
std::cerr << "  <source vertex       >" << u.to_string() << std::endl;
std::cerr << "  <target vertex before>" << v.to_string() << std::endl;
#endif

            term_type value = m_pbes_rewriter(data::data_variable_map_replace(e.condition, u.constraints));
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cerr << "\nEvaluated condition " << core::pp(data::data_variable_map_replace(e.condition, u.constraints)) << " to " << core::pp(value) << std::endl;
#endif
            if (!tr::is_false(value) && !tr::is_true(value))
            {
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cerr << "\nCould not evaluate condition " << core::pp(data::data_variable_map_replace(e.condition, u.constraints)) << " to true or false";
#endif
            }
            if (!tr::is_false(value))
            {
              bool changed = v.update(e.right.parameters(), u.constraints, m_data_rewriter, name_generator);
              if (changed)
              {
                todo.push_back(v.variable);
              }
            }
#ifdef MCRL2_PBES_CONSTELM_DEBUG
std::cerr << "  <target vertex after >" << v.to_string() << std::endl;
#endif
          }
        }

        if (mcrl2::core::gsDebug)
        {
          std::cerr << "\n--- final vertices ---" << std::endl;
          print_vertices();
        }

        // print the parameters that will be removed
        if (mcrl2::core::gsVerbose)
        {
          std::cerr << "\nremoving the following constant parameters and equations:" << std::endl;
          for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
          {
            const vertex& u = i->second;
            if (u.constraints.empty())
            {
              std::cerr << "  equation " << core::pp(u.variable) << std::endl;
              m_removed[u.variable].insert(u.variable.parameters().begin(), u.variable.parameters().end());
            }
            else
            {
              std::vector<variable_type> removed = u.constant_parameters();
              for (typename std::vector<variable_type>::iterator j = removed.begin(); j != removed.end(); ++j)
              {
                std::cerr << "  parameter (" << mcrl2::core::pp(u.variable.name()) << ", " << core::pp(*j) << ")" << std::endl;
                m_removed[u.variable].insert(*j);
              }
            }
          }
        }

        // remove the constant parameters
        std::map<string_type, std::vector<int> > to_be_removed_variables;
        for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          string_type name = i->variable().name();
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
          string_type name = i->variable().name();
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

      const std::map<propositional_variable_decl_type, std::set<variable_type> >& removed_variables() const
      {
        return m_removed;
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_CONSTELM_H
