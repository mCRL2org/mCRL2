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
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/remove_parameters.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

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

namespace atermpp
{
/// \cond INTERNAL_DOCS
template<typename Term>
struct aterm_traits<mcrl2::pbes_system::detail::true_false_pair<Term> >
{
  static void protect(const mcrl2::pbes_system::detail::true_false_pair<Term>& t)
  {
    t.TC.protect();
    t.FC.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::true_false_pair<Term>& t)
  {
    t.TC.unprotect();
    t.FC.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::true_false_pair<Term>& t)
  {
    t.TC.mark();
    t.FC.mark();
  }
};
} // namespace atermpp

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
  return ATermAppl(v) < ATermAppl(w);
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
    for (typename std::vector<true_false_pair<Term> >::const_iterator i = c.begin(); i != c.end(); ++i)
    {
      result = utilities::optimized_and(result, utilities::optimized_not(i->TC));
      result = utilities::optimized_and(result, utilities::optimized_not(i->FC));
    }
    return result;
  }
};

template <typename Term>
struct edge_condition_visitor: public pbes_expression_visitor<Term, constelm_edge_condition<Term> >
{
  typedef pbes_expression_visitor<Term, constelm_edge_condition<Term> > super;
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

  /// \brief Visit data_expression node
  /// \param e A term
  /// \param d A data term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_data_expression(const term_type& /* e */, const data_term_type& d, edge_condition& ec)
  {
    ec.TC = d;
    ec.FC = utilities::optimized_not(d);
    return this->stop_recursion;
  }

  /// \brief Visit true node
  /// \param e A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_true(const term_type& /* e */, edge_condition& ec)
  {
    ec.TC = tr::true_();
    ec.FC = tr::false_();
    return this->stop_recursion;
  }

  /// \brief Visit false node
  /// \param e A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_false(const term_type& /* e */, edge_condition& ec)
  {
    ec.TC = tr::false_();
    ec.FC = tr::true_();
    return this->stop_recursion;
  }

  /// \brief Visit not node
  /// \param e A term
  /// \param arg A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_not(const term_type& /* e */, const term_type& arg, edge_condition& ec)
  {
    edge_condition ec_arg;
    super::visit(arg, ec_arg);
    ec.TC = ec_arg.FC;
    ec.FC = ec_arg.TC;
    ec.condition = ec_arg.condition;
    return this->stop_recursion;
  }

  /// \brief Visit and node
  /// \param e A term
  /// \param left A term
  /// \param right A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_and(const term_type& /* e */, const term_type& left, const term_type&  right, edge_condition& ec)
  {
    edge_condition ec_left;
    super::visit(left, ec_left);
    edge_condition ec_right;
    super::visit(right, ec_right);
    ec.TC = utilities::optimized_and(ec_left.TC, ec_right.TC);
    ec.FC = utilities::optimized_or(ec_left.FC, ec_right.FC);
    merge_conditions(ec_left, ec_right, ec);
    return this->stop_recursion;
  }

  /// \brief Visit or node
  /// \param e A term
  /// \param left A term
  /// \param right A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_or(const term_type& /* e */, const term_type&  left, const term_type&  right, edge_condition& ec)
  {
    edge_condition ec_left;
    super::visit(left, ec_left);
    edge_condition ec_right;
    super::visit(right, ec_right);
    ec.TC = utilities::optimized_or(ec_left.TC, ec_right.TC);
    ec.FC = utilities::optimized_and(ec_left.FC, ec_right.FC);
    merge_conditions(ec_left, ec_right, ec);
    return this->stop_recursion;
  }

  /// \brief Visit imp node
  /// \param e A term
  /// \param left A term
  /// \param right A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_imp(const term_type& /* e */, const term_type&  left, const term_type&  right, edge_condition& ec)
  {
    edge_condition ec_left;
    super::visit(left, ec_left);
    edge_condition ec_right;
    super::visit(right, ec_right);
    ec.TC = utilities::optimized_or(ec_left.FC, ec_right.TC);
    ec.FC = utilities::optimized_and(ec_left.TC, ec_right.FC);
    merge_conditions(ec_left, ec_right, ec);
    return this->stop_recursion;
  }

  /// \brief Visit forall node
  /// \param e A term
  /// \param variables A sequence of variables
  /// \param expr A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_forall(const term_type& /* e */, const variable_sequence_type& variables, const term_type& expr, edge_condition& ec)
  {
    super::visit(expr, ec);
    for (typename condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      std::for_each(i->second.begin(), i->second.end(), apply_forall<Term>(variables));
    }
    return this->stop_recursion;
  }

  /// \brief Visit exists node
  /// \param e A term
  /// \param variables A sequence of variables
  /// \param expr A term
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_exists(const term_type& /* e */, const variable_sequence_type&  variables, const term_type& expr, edge_condition& ec)
  {
    super::visit(expr, ec);
    for (typename condition_map::iterator i = ec.condition.begin(); i != ec.condition.end(); ++i)
    {
      i->second.push_back(ec.TCFC());
      std::for_each(i->second.begin(), i->second.end(), apply_exists<Term>(variables));
    }
    return this->stop_recursion;
  }

  /// \brief Visit propositional_variable node
  /// \param e A term
  /// \param v A propositional variable
  /// \param ec An edge condition
  /// \return The result of visiting the node
  bool visit_propositional_variable(const term_type& /* e */, const propositional_variable_type& v, edge_condition& ec)
  {
    ec.TC = tr::false_();
    ec.FC = tr::false_();
    std::vector<true_false_pair<Term> > c;
    c.push_back(true_false_pair<Term>(tr::false_(), tr::false_()));
    ec.condition.insert(std::make_pair(v, c));
    return this->stop_recursion;
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

    /// \brief The visitor type for edge conditions
    typedef typename detail::edge_condition_visitor<Term> edge_condition_visitor;

    /// \brief The edge condition type
    typedef typename edge_condition_visitor::edge_condition edge_condition;

    /// \brief The edge condition map type
    typedef typename edge_condition_visitor::condition_map condition_map;

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
        /// \param l A propositional variable declaration
        /// \param r A propositional variable
        /// \param c A term
        edge(propositional_variable_decl_type src, propositional_variable_type tgt, term_type c = pbes_expr::true_())
          : term_type(c), m_source(src), m_target(tgt)
        {}

        /// \brief Returns a string representation of the edge.
        /// \return A string representation of the edge.
        std::string to_string() const
        {
          std::ostringstream out;
          out << "(" << mcrl2::core::pp(m_source.name()) << ", " << mcrl2::core::pp(m_target.name()) << ")  label = " << pbes_system::pp(m_target) << "  condition = " << mcrl2::data::pp(condition());
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
        /// \param v A propositional variable declaration
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
        bool is_constant(variable_type v) const
        {
          typename constraint_map::const_iterator i = m_constraints.find(v);
          return i != m_constraints.end() && !core::term_traits<data_term_type>::is_variable(i->second);
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
          out << mcrl2::data::pp(m_variable) << "  assertions = ";
          for (typename constraint_map::const_iterator i = m_constraints.begin(); i != m_constraints.end(); ++i)
          {
            std::string lhs = mcrl2::data::pp(i->first);
            std::string rhs = mcrl2::data::pp(i->second);
            out << "{" << lhs << " := " << rhs << "} ";
          }
          return out.str();
        }

        /// \brief Assign new values to the parameters of this vertex, and update the constraints accordingly.
        /// The new values have a number of constraints.
        bool update(data_term_sequence_type e, const constraint_map& e_constraints, DataRewriter datar)
        {
          if (e.empty())
          {
            return false;
          }

          bool changed = false;

          typename data_term_sequence_type::iterator i;
          typename variable_sequence_type::iterator j;
          variable_sequence_type params = m_variable.parameters();

          if (m_constraints.empty())
          {
            for (i = e.begin(), j = params.begin(); i != e.end(); ++i, ++j)
            {
              // TODO: why not use R(t, sigma) interface here?
              data_term_type e1 = datar(*i, data::make_map_substitution(e_constraints));
              if (core::term_traits<data_term_type>::is_constant(e1))
              {
                m_constraints[*j] = e1;
              }
              else
              {
                m_constraints[*j] = *j;
              }
            }
            changed = true;
          }
          else
          {
            for (i = e.begin(), j = params.begin(); i != e.end(); ++i, ++j)
            {
              typename constraint_map::iterator k = m_constraints.find(*j);
              assert(k != m_constraints.end());
              data_term_type& ci = k->second;
              if (ci == *j)
              {
                continue;
              }
              // TODO: why not use R(t, sigma) interface here?
              data_term_type ei = datar(*i, data::make_map_substitution(e_constraints));
              if (ci != ei)
              {
                ci = *j;
                changed = true;
              }
            }
          }
          return changed;
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
      out << "\nEvaluated condition " << pbes_system::pp(pbes_system::replace_free_variables(e.condition(), data::make_map_substitution(u.constraints()))) << " to " << data::pp(value) << std::endl;
      return out.str();
    }

    std::string print_evaluation_failure(const edge& e, const vertex& u)
    {
      std::ostringstream out;
      out << "\nCould not evaluate condition " << pbes_system::pp(pbes_system::replace_free_variables(e.condition(), data::make_map_substitution(u.constraints()))) << " to true or false";
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
      for (typename std::map<string_type, std::vector<size_t> >::const_iterator i = m_redundant_parameters.begin(); i != m_redundant_parameters.end(); ++i)
      {
        const vertex& v = m_vertices.find(i->first)->second;
        std::vector<variable_type>& variables = result[v.variable()];
        for (std::vector<size_t>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
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
    /// \param name_generator A generator for fresh identifiers
    /// The call \p name_generator() should return an identifier that doesn't appear
    /// in the pbes \p p
    /// \param compute_conditions If true, propagation conditions are computed. Note
    /// that the currently implementation has exponential behavior.
    template <typename Container>
    void run(pbes<Container>& p, bool compute_conditions = false)
    {
      m_vertices.clear();
      m_edges.clear();
      m_redundant_parameters.clear();

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

          // TODO: why not use R(t, sigma) interface here?
          term_type value = m_pbes_rewriter(e.condition(), data::make_map_substitution(u.constraints()));
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

      mCRL2log(log::debug) << "\n--- final vertices ---\n" << print_vertices();

      // compute the redundant parameters and the redundant equations
      for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
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
      for (typename Container::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        string_type name = i->variable().name();
        vertex& v = m_vertices[name];

        if (!v.constraints().empty())
        {
          *i = pbes_equation(
                 i->symbol(),
                 i->variable(),
                 pbes_system::replace_free_variables(i->formula(), data::make_map_substitution(v.constraints()))
               );
        }
      }

      // remove the redundant parameters
      remove_parameters(p, m_redundant_parameters);

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
