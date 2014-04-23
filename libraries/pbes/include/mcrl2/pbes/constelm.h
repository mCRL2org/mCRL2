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

#include <sstream>
#include <iostream>
#include <utility>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

inline
data::mutable_map_substitution<> make_constelm_substitution(const std::map<data::variable, data::data_expression>& m)
{
  data::mutable_map_substitution<> result;
  for (auto i = m.begin(); i != m.end(); ++i)
  {
    result[i->first] = i->second;
  }
  return result;
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

} // namespace detail

} // namespace mcrl2

} // namespace pbes_system

namespace mcrl2
{

namespace pbes_system
{

/// \cond INTERNAL_DOCS
namespace detail
{
/// \brief Compares two terms
/// \param v A term
/// \param w A term
/// \return True if v is less than w
inline
bool less_term(atermpp::aterm_appl v, atermpp::aterm_appl w)
{
  return v < w;
}

template <typename Term>
struct apply_exists
{
  typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<Term> tr;

  variable_sequence_type variables_;

  apply_exists(variable_sequence_type variables)
    : variables_(variables)
  {}

  /// \brief Function call operator
  /// \param p A true-false pair
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

  /// \brief Function call operator
  /// \param p A true-false pair
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

  constelm_edge_condition(const term_type& tc, const term_type& fc)
    : TC(tc), FC(fc)
  {}

  /// \brief Returns the true-false pair corresponding to the edge condition
  /// \return The true-false pair corresponding to the edge condition
  true_false_pair<Term> TCFC() const
  {
    return true_false_pair<Term>(TC, FC);
  }

  /// \brief Returns the condition
  /// \param c A sequence of true-false pairs
  /// \return The condition
  term_type compute_condition(const std::vector<true_false_pair<Term> >& c) const
  {
    term_type result = tr::true_();
    for (auto i = c.begin(); i != c.end(); ++i)
    {
      result = utilities::optimized_and(result, utilities::optimized_not(i->TC));
      result = utilities::optimized_and(result, utilities::optimized_not(i->FC));
    }
    return result;
  }
};

struct edge_condition_traverser: public pbes_expression_traverser<edge_condition_traverser>
{
  typedef pbes_expression_traverser<edge_condition_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef constelm_edge_condition<pbes_expression> edge_condition;
  typedef edge_condition::condition_map condition_map;
  typedef core::term_traits<pbes_expression> tr;

  std::vector<edge_condition> condition_stack;

  void push(const edge_condition& x)
  {
    condition_stack.push_back(x);
  }

  edge_condition& top()
  {
    return condition_stack.back();
  }

  const edge_condition& top() const
  {
    return condition_stack.back();
  }

  edge_condition pop()
  {
    edge_condition result = top();
    condition_stack.pop_back();
    return result;
  }

  // N.B. As a side effect ec1 and ec2 are changed!!!
  void merge_conditions(edge_condition& ec1,
                        edge_condition& ec2,
                        edge_condition& ec
                       )
  {
    for (auto i = ec1.condition.begin(); i != ec1.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      ec.condition.insert(*i);
    }
    for (auto i = ec2.condition.begin(); i != ec2.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      ec.condition.insert(*i);
    }
  }

  void leave(const data::data_expression& x)
  {
    push(edge_condition(x, utilities::optimized_not(x)));
  }

  void leave(const true_&)
  {
    push(edge_condition(tr::true_(), tr::false_()));
  }

  void leave(const false_&)
  {
    push(edge_condition(tr::false_(), tr::true_()));
  }

  void leave(const not_&)
  {
    edge_condition ec = pop();
    std::swap(ec.TC, ec.FC);
    push(ec);
  }

  void leave(const and_&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(utilities::optimized_and(ec_left.TC, ec_right.TC), utilities::optimized_or(ec_left.FC, ec_right.FC));
    merge_conditions(ec_left, ec_right, ec);
    push(ec);
  }

  void leave(const or_&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(utilities::optimized_or(ec_left.TC, ec_right.TC), utilities::optimized_and(ec_left.FC, ec_right.FC));
    merge_conditions(ec_left, ec_right, ec);
    push(ec);
  }

  void leave(const imp&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(utilities::optimized_or(ec_left.FC, ec_right.TC), utilities::optimized_and(ec_left.TC, ec_right.FC));
    merge_conditions(ec_left, ec_right, ec);
    push(ec);
  }

  void leave(const forall& x)
  {
    edge_condition ec = pop();
    for (auto i = ec.condition.begin(); i != ec.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      std::for_each(i->second.begin(), i->second.end(), apply_forall<pbes_expression>(x.variables()));
    }
    push(ec);
  }

  void leave(const exists& x)
  {
    edge_condition ec = pop();
    for (auto i = ec.condition.begin(); i != ec.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      std::for_each(i->second.begin(), i->second.end(), apply_exists<pbes_expression>(x.variables()));
    }
    push(ec);
  }

  void leave(const propositional_variable_instantiation& x)
  {
    edge_condition ec(tr::false_(), tr::false_());
    std::vector<true_false_pair<pbes_expression> > c;
    c.push_back(true_false_pair<pbes_expression>(tr::false_(), tr::false_()));
    ec.condition.insert(std::make_pair(x, c));
    push(ec);
  }

  const edge_condition& result() const
  {
    assert(condition_stack.size() == 1);
    return top();
  }
};

template <typename Container, typename Predicate>
/// \brief Removes elements from a container
/// \param container A container
/// \param pred All elements that satisfy the predicate pred are removed
/// Note: this implementation is very inefficient!
void remove_elements(Container& container, Predicate pred)
{
  std::vector<typename Container::value_type> result;
  for (typename Container::iterator i = container.begin(); i != container.end(); ++i)
  {
    if (!pred(*i))
    {
      result.push_back(*i);
    }
  }
  container = Container(result.begin(), result.end());
}

template <typename Variable>
struct equation_is_contained_in
{
  const std::set<Variable>& m_variables;

  equation_is_contained_in(const std::set<Variable>& variables)
    : m_variables(variables)
  {}

  template <typename Equation>
  bool operator()(const Equation& e)
  {
    return m_variables.find(e.variable()) != m_variables.end();
  }
};

template <typename MapContainer>
void print_constraint_map(const MapContainer& constraints)
{
  for (typename MapContainer::const_iterator i = constraints.begin(); i != constraints.end(); ++i)
  {
    std::string lhs = data::pp(i->first);
    std::string rhs = data::pp(i->second);
    std::cout << "{" << lhs << " := " << rhs << "} ";
  }
}

} // namespace detail
/// \endcond

/// \brief Algorithm class for the constelm algorithm
template <typename Term, typename DataRewriter, typename PbesRewriter>
class pbes_constelm_algorithm
{
  public:
    /// \brief The term type
    typedef typename core::term_traits<Term>::term_type term_type;

    /// \brief The variable type
    typedef typename core::term_traits<Term>::variable_type variable_type;

    /// \brief The variable sequence type
    typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;

    /// \brief The data term type
    typedef typename core::term_traits<Term>::data_term_type data_term_type;

    /// \brief The data term sequence type
    typedef typename core::term_traits<Term>::data_term_sequence_type data_term_sequence_type;

    /// \brief The string type
    typedef typename core::term_traits<Term>::string_type string_type;

    /// \brief The propositional variable declaration type
    typedef typename core::term_traits<Term>::propositional_variable_decl_type propositional_variable_decl_type;

    /// \brief The propositional variable instantiation type
    typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

    /// \brief The edge condition type
    typedef detail::edge_condition_traverser::edge_condition edge_condition;

    /// \brief The edge condition map type
    typedef detail::edge_condition_traverser::condition_map condition_map;

    /// \brief The term traits
    typedef typename core::term_traits<Term> tr;

  protected:
    /// \brief A map with constraints on the vertices of the graph
    typedef std::map<variable_type, data_term_type> constraint_map;

    /// \brief Compares data expressions for equality.
    DataRewriter m_data_rewriter;

    /// \brief Compares data expressions for equality.
    PbesRewriter m_pbes_rewriter;

    /// \brief Represents an edge of the dependency graph. The assignments are stored
    /// implicitly using the 'right' parameter. The condition determines under
    /// what circumstances the influence of the edge is propagated to its target
    /// vertex.
    //
    // N.B. The attribute condition "term_type condition;" needs to be protected.
    // This is achieved by deriving from term_type. This is very ugly, but AFAIK
    // this is the least destructive solution to garbage collection problems.
    // Note that source and target are protected elsewhere.
    class edge: public term_type
    {
      protected:
        /// \brief The propositional variable at the source of the edge
        propositional_variable_decl_type m_source;

        /// \brief The propositional variable instantiation that determines the target of the edge
        propositional_variable_type m_target;

      public:
        /// \brief Constructor
        edge()
        {}

        /// \brief Constructor
        /// \param src A propositional variable declaration
        /// \param tgt A propositional variable
        /// \param c A term
        edge(propositional_variable_decl_type src, propositional_variable_type tgt, term_type c = pbes_expr::true_())
          : term_type(c), m_source(src), m_target(tgt)
        {}

        /// \brief Returns a string representation of the edge.
        /// \return A string representation of the edge.
        std::string to_string() const
        {
          std::ostringstream out;
          out << "(" << m_source.name() << ", " << m_target.name() << ")  label = " << m_target << "  condition = " << condition();
          return out.str();
        }

        /// \brief The propositional variable at the source of the edge
        const propositional_variable_decl_type& source() const
        {
          return m_source;
        }

        /// \brief The propositional variable instantiation that determines the target of the edge
        const propositional_variable_type& target() const
        {
          return m_target;
        }

        /// \brief The condition of the edge
        const term_type& condition() const
        {
          return *this;
        }
    };

    /// \brief Represents a vertex of the dependency graph.
    class vertex
    {
      protected:
        /// \brief The propositional variable that corresponds to the vertex
        propositional_variable_decl_type m_variable;

        /// \brief Maps data variables to data expressions. If the right hand side is a data
        /// variable, it means that it represents NaC ("not a constant").
        constraint_map m_constraints;

      public:
        /// \brief Constructor
        vertex()
        {}

        /// \brief Constructor
        /// \param x A propositional variable declaration
        vertex(propositional_variable_decl_type x)
          : m_variable(x)
        {}

        /// \brief The propositional variable that corresponds to the vertex
        const propositional_variable_decl_type& variable() const
        {
          return m_variable;
        }

        /// \brief Maps data variables to data expressions. If the right hand side is a data
        /// variable, it means that it represents NaC ("not a constant").
        const constraint_map& constraints() const
        {
          return m_constraints;
        }

        /// \brief Returns true if the data variable v has been assigned a constant expression.
        /// \param v A variable
        /// \return True if the data variable v has been assigned a constant expression.
        bool is_constant(const variable_type& v) const
        {
          auto i = m_constraints.find(v);
          return i != m_constraints.end() && !core::term_traits<data_term_type>::is_variable(i->second);
        }

        /// \brief Returns true if the expression x has the value undefined_data_expression or if x is a constant data expression.
        /// \param x A
        /// \return True if the data variable v has been assigned a constant expression.
        bool is_constant_expression(const data_term_type& x) const
        {
          return x == data::undefined_data_expression() || core::term_traits<data_term_type>::is_constant(x);
        }

        /// \brief Returns the constant parameters of this vertex.
        /// \return The constant parameters of this vertex.
        std::vector<variable_type> constant_parameters() const
        {
          std::vector<variable_type> result;
          variable_sequence_type parameters(m_variable.parameters());
          for (typename variable_sequence_type::iterator i = parameters.begin(); i != parameters.end(); ++i)
          {
            if (is_constant(*i))
            {
              result.push_back(*i);
            }
          }
          return result;
        }

        /// \brief Returns the indices of the constant parameters of this vertex.
        /// \return The indices of the constant parameters of this vertex.
        std::vector<size_t> constant_parameter_indices() const
        {
          std::vector<size_t> result;
          int index = 0;
          variable_sequence_type parameters(m_variable.parameters());
          for (typename variable_sequence_type::iterator i = parameters.begin(); i != parameters.end(); ++i, index++)
          {
            if (is_constant(*i))
            {
              result.push_back(index);
            }
          }
          return result;
        }

        /// \brief Returns a string representation of the vertex.
        /// \return A string representation of the vertex.
        std::string to_string() const
        {
          std::ostringstream out;
          out << m_variable << "  assertions = ";
          for (typename constraint_map::const_iterator i = m_constraints.begin(); i != m_constraints.end(); ++i)
          {
            out << "{" << i->first << " := " << i->second << "} ";
          }
          return out.str();
        }

        /// \brief Assign new values to the parameters of this vertex, and update the constraints accordingly.
        /// The new values have a number of constraints.
        bool update(data_term_sequence_type e, const constraint_map& e_constraints, DataRewriter datar)
        {
          bool changed = false;

          variable_sequence_type params = m_variable.parameters();

          if (m_constraints.empty())
          {
            if (e.empty())
            {
              m_constraints[data::undefined_variable()] = data::undefined_data_expression();
            }
            else
            {
              auto j = params.begin();
              for (auto i = e.begin(); i != e.end(); ++i, ++j)
              {
                data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(e_constraints);
                data_term_type e1 = datar(*i, sigma);
                if (is_constant_expression(e1))
                {
                  m_constraints[*j] = e1;
                }
                else
                {
                  m_constraints[*j] = *j;
                }
              }
            }
            changed = true;
          }
          else
          {
            auto j = params.begin();
            for (auto i = e.begin(); i != e.end(); ++i, ++j)
            {
              auto k = m_constraints.find(*j);
              assert(k != m_constraints.end());
              data_term_type& ci = k->second;
              if (ci == *j)
              {
                continue;
              }
              data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(e_constraints);
              data_term_type ei = datar(*i, sigma);
              if (ci != ei)
              {
                ci = *j;
                changed = true;
              }
            }
          }
          return changed;
        }

        // Removes assignments to undefined_variable() from constraints()
        void remove_undefined_values()
        {
          auto i = m_constraints.find(data::undefined_variable());
          if (i != m_constraints.end())
          {
            m_constraints.erase(i);
          }
        }
    };

    /// \brief The storage type for vertices
    typedef std::map<string_type, vertex> vertex_map;

    /// \brief The storage type for edges
    typedef std::map<string_type, std::vector<edge> > edge_map;

    /// \brief The vertices of the dependency graph. They are stored in a map, to
    /// support searching for a vertex.
    vertex_map m_vertices;

    /// \brief The edges of the dependency graph. They are stored in a map, to
    /// easily access all out-edges corresponding to a particular vertex.
    edge_map m_edges;

    /// \brief The redundant parameters.
    std::map<string_type, std::vector<size_t> > m_redundant_parameters;

    /// \brief Logs the vertices of the dependency graph.
    std::string print_vertices() const
    {
      std::ostringstream out;
      for (typename vertex_map::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        out << i->second.to_string() << std::endl;
      }
      return out.str();
    }

    /// \brief Logs the edges of the dependency graph.
    std::string print_edges()
    {
      std::ostringstream out;
      for (typename edge_map::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
      {
        for (typename std::vector<edge>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
          out << j->to_string() << std::endl;
        }
      }
      return out.str();
    }

    std::string print_todo_list(const std::deque<propositional_variable_decl_type>& todo)
    {
      std::ostringstream out;
      out << "\n<todo list> [";
      for (typename std::deque<propositional_variable_decl_type>::const_iterator i = todo.begin(); i != todo.end(); ++i)
      {
        if (i != todo.begin())
        {
          out << ", ";
        }
        out << core::pp(i->name());
      }
      out << "]" << std::endl;
      return out.str();
    }

    std::string print_edge_update(const edge& e, const vertex& u, const vertex& v)
    {
      std::ostringstream out;
      out << "\n<updating edge>" << e.to_string() << std::endl;
      out << "  <source vertex       >" << u.to_string() << std::endl;
      out << "  <target vertex before>" << v.to_string() << std::endl;
      return out.str();
    }

    std::string print_condition(const edge& e, const vertex& u, const term_type& value)
    {
      std::ostringstream out;
      data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(u.constraints());
      out << "\nEvaluated condition " << e.condition() << sigma << " to " << value << std::endl;
      return out.str();
    }

    std::string print_evaluation_failure(const edge& e, const vertex& u)
    {
      std::ostringstream out;
      data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(u.constraints());
      out << "\nCould not evaluate condition " << e.condition() << sigma << " to true or false";
      return out.str();
    }

  public:

    /// \brief Constructor.
    /// \param datar A data rewriter
    /// \param pbesr A PBES rewriter
    pbes_constelm_algorithm(DataRewriter datar, PbesRewriter pbesr)
      : m_data_rewriter(datar), m_pbes_rewriter(pbesr)
    {}

    /// \brief Returns the parameters that have been removed by the constelm algorithm
    /// \return The removed parameters
    std::map<propositional_variable_decl_type, std::vector<variable_type> > redundant_parameters() const
    {
      std::map<propositional_variable_decl_type, std::vector<variable_type> > result;
      for (auto i = m_redundant_parameters.begin(); i != m_redundant_parameters.end(); ++i)
      {
        const vertex& v = m_vertices.find(i->first)->second;
        std::vector<variable_type>& variables = result[v.variable()];
        for (auto j = i->second.begin(); j != i->second.end(); ++j)
        {
          // std::advance doesn't work for aterm lists :-(
          variable_sequence_type parameters(v.variable().parameters());
          typename variable_sequence_type::iterator k = parameters.begin();
          for (size_t i = 0; i < *j; i++)
          {
            ++k;
          }
          variables.push_back(*k);
        }
      }
      return result;
    }

    /// \brief Runs the constelm algorithm
    /// \param p A pbes
    /// \param compute_conditions If true, propagation conditions are computed. Note
    /// that the currently implementation has exponential behavior.
    void run(pbes& p, bool compute_conditions = false)
    {
      m_vertices.clear();
      m_edges.clear();
      m_redundant_parameters.clear();

      // compute the vertices and edges of the dependency graph
      for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        string_type name = i->variable().name();
        m_vertices[name] = vertex(i->variable());

        if (compute_conditions)
        {
          // use an edge_condition_traverser to compute the edges
          detail::edge_condition_traverser f;
          f(i->formula());
          edge_condition ec = f.result();
          if (!ec.condition.empty())
          {
            std::vector<edge>& edges = m_edges[name];
            for (auto j = ec.condition.begin(); j != ec.condition.end(); ++j)
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
          std::set<propositional_variable_type> inst = find_propositional_variable_instantiations(i->formula());
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
      propositional_variable_instantiation init = p.initial_state();
      std::deque<propositional_variable_decl_type> todo;
      data_term_sequence_type e = init.parameters();
      vertex& u = m_vertices[init.name()];
      u.update(e, constraint_map(), m_data_rewriter);
      todo.push_back(u.variable());

      mCRL2log(log::debug) << "\n--- initial vertices ---\n" << print_vertices();
      mCRL2log(log::debug) << "\n--- edges ---\n" << print_edges();

      // propagate constraints over the edges until the todo list is empty
      while (!todo.empty())
      {
        mCRL2log(log::debug) << print_todo_list(todo);
        propositional_variable_decl_type var = todo.front();

        // remove all occurrences of var from todo
        todo.erase(std::remove(todo.begin(), todo.end(), var), todo.end());

        const vertex& u = m_vertices[var.name()];
        std::vector<edge>& u_edges = m_edges[var.name()];

        for (typename std::vector<edge>::const_iterator ei = u_edges.begin(); ei != u_edges.end(); ++ei)
        {
          const edge& e = *ei;
          vertex& v = m_vertices[e.target().name()];
          mCRL2log(log::debug) << print_edge_update(e, u, v);

          data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(u.constraints());
          term_type value = m_pbes_rewriter(e.condition(), sigma);
          mCRL2log(log::debug) << print_condition(e, u, value);

          if (!tr::is_false(value) && !tr::is_true(value))
          {
            mCRL2log(log::debug) << print_evaluation_failure(e, u);
          }
          if (!tr::is_false(value))
          {
            bool changed = v.update(e.target().parameters(), u.constraints(), m_data_rewriter);
            if (changed)
            {
              todo.push_back(v.variable());
            }
          }
          mCRL2log(log::debug) << "  <target vertex after >" << v.to_string() << "\n";
        }
      }

      // remove undefined values from constraints
      for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        i->second.remove_undefined_values();
      }

      mCRL2log(log::debug) << "\n--- final vertices ---\n" << print_vertices();

      // compute the redundant parameters and the redundant equations
      for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        string_type name = i->variable().name();
        vertex& v = m_vertices[name];
        if (!v.constraints().empty())
        {
          std::vector<size_t> r = v.constant_parameter_indices();
          if (!r.empty())
          {
            m_redundant_parameters[name] = r;
          }
        }
      }

      // Apply the constraints to the equations.
      for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        string_type name = i->variable().name();
        vertex& v = m_vertices[name];

        if (!v.constraints().empty())
        {
          data::mutable_map_substitution<> sigma = detail::make_constelm_substitution(v.constraints());
          *i = pbes_equation(
                 i->symbol(),
                 i->variable(),
                 pbes_system::replace_free_variables(i->formula(), sigma)
               );
        }
      }

      // remove the redundant parameters
      pbes_system::algorithms::remove_parameters(p, m_redundant_parameters);

      // print the parameters and equation that are removed
      if (mCRL2logEnabled(log::verbose))
      {
        mCRL2log(log::verbose) << "\nremoved the following constant parameters:" << std::endl;
        std::map<propositional_variable_decl_type, std::vector<variable_type> > v = redundant_parameters();
        for (typename std::map<propositional_variable_decl_type, std::vector<variable_type> >::iterator i = v.begin(); i != v.end(); ++i)
        {
          for (typename std::vector<variable_type>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
          {
            mCRL2log(log::verbose) << "  (" << mcrl2::core::pp(i->first.name()) << ", " << data::pp(*j) << ")" << std::endl;
          }
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_CONSTELM_H
