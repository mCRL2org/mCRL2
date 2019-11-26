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

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

inline
void make_constelm_substitution(const std::map<data::variable, data::data_expression>& m, data::rewriter::substitution_type& result)
{
  for (const auto& i : m)
  {
    result[i.first] = i.second;
  }
}

class quantified_variable
{
protected:
  bool m_is_forall;
  data::variable m_var;

public:
  quantified_variable(bool is_forall, const data::variable& var)
  : m_is_forall(is_forall)
  , m_var(var)
  {}

  bool is_forall() const
  {
    return m_is_forall;
  }

  const data::variable& variable() const
  {
    return m_var;
  }

  bool operator==(const quantified_variable& other) const
  {
    return m_is_forall == other.m_is_forall && m_var == other.m_var;
  }

  bool operator<(const quantified_variable& other) const
  {
    return m_is_forall < other.m_is_forall || (m_is_forall == other.m_is_forall && m_var < other.m_var);
  }

  pbes_expression make_expr(const pbes_expression& expr) const
  {
    return m_is_forall ? pbes_expression(forall(data::variable_list({m_var}), expr)) : pbes_expression(exists(data::variable_list({m_var}), expr));
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << (is_forall() ? "forall " : "exists ") << variable() << ". ";
    return out.str();
  }
};

struct constelm_edge_condition
{
  typedef std::multimap<std::pair<std::list<quantified_variable>, propositional_variable_instantiation>, std::set<data::data_expression> > condition_map;

  data::data_expression TC;
  data::data_expression FC;
  condition_map condition;  // condT + condF

  constelm_edge_condition(const data::data_expression& tc, const data::data_expression& fc)
    : TC(tc), FC(fc)
  {}
};

struct edge_condition_traverser: public pbes_expression_traverser<edge_condition_traverser>
{
  typedef pbes_expression_traverser<edge_condition_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  typedef constelm_edge_condition edge_condition;
  typedef edge_condition::condition_map condition_map;
  typedef std::list<detail::quantified_variable> qvar_list;

  std::vector<edge_condition> condition_stack;
  std::list<pbes_expression> quantified_context;

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
  void merge_conditions(edge_condition& ec1, bool negate1,
                        edge_condition& ec2, bool negate2,
                        edge_condition& ec
                       )
  {
    for (auto& i: ec1.condition)
    {
      i.second.insert(negate2 ? ec2.FC : ec2.TC);
      ec.condition.insert(i);
    }
    for (auto& i: ec2.condition)
    {
      i.second.insert(negate1 ? ec1.FC : ec1.TC);
      ec.condition.insert(i);
    }
  }

  // enter functions related to maintaining the quantfier scope
  void enter(const not_&)
  {
    quantified_context.clear();
  }

  void enter(const and_&)
  {
    quantified_context.clear();
  }

  void enter(const or_&)
  {
    quantified_context.clear();
  }

  void enter(const imp&)
  {
    quantified_context.clear();
  }

  void enter(const forall& x)
  {
    quantified_context.push_back(x);
  }

  void enter(const exists& x)
  {
    quantified_context.push_back(x);
  }

  // leave functions, mostly used to build conditions
  void leave(const data::data_expression& x)
  {
    push(edge_condition(x, data::optimized_not(x)));
  }

  void leave(const not_&)
  {
    std::swap(top().TC, top().FC);
  }

  void leave(const and_&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(data::optimized_and(ec_left.TC, ec_right.TC), data::optimized_or(ec_left.FC, ec_right.FC));
    merge_conditions(ec_left, false, ec_right, false, ec);
    push(ec);
  }

  void leave(const or_&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(data::optimized_or(ec_left.TC, ec_right.TC), data::optimized_and(ec_left.FC, ec_right.FC));
    merge_conditions(ec_left, true, ec_right, true, ec);
    push(ec);
  }

  void leave(const imp&)
  {
    edge_condition ec_right = pop();
    edge_condition ec_left = pop();
    edge_condition ec(data::optimized_or(ec_left.FC, ec_right.TC), data::optimized_and(ec_left.TC, ec_right.FC));
    merge_conditions(ec_left, false, ec_right, true, ec);
    push(ec);
  }

  void leave(const forall& x)
  {
    // build conditions
    edge_condition ec = pop();
    for (auto& [X_e, cond_set]: ec.condition)
    {
      std::set<data::data_expression> new_conditions;
      for(const data::data_expression& e: cond_set)
      {
        new_conditions.insert(data::optimized_exists(x.variables(), e, true));
      }
      cond_set = std::move(new_conditions);
      cond_set.insert(data::optimized_forall(x.variables(), ec.TC, true));
    }
    push(ec);

    // maintain quantifier scope
    if(quantified_context.back() == x)
    {
      quantified_context.pop_back();
    }
  }

  void leave(const exists& x)
  {
    // build conditions
    edge_condition ec = pop();
    for (auto& [X_e, cond_set]: ec.condition)
    {
      std::set<data::data_expression> new_conditions;
      for(const data::data_expression& e: cond_set)
      {
        new_conditions.insert(data::optimized_exists(x.variables(), e, true));
      }
      cond_set = std::move(new_conditions);
      cond_set.insert(data::optimized_forall(x.variables(), ec.FC, true));
    }
    push(ec);

    // maintain quantifier scope
    if(quantified_context.back() == x)
    {
      quantified_context.pop_back();
    }
  }

  void leave(const propositional_variable_instantiation& x)
  {
    // Build list of qvars from quantifier scope
    qvar_list qvars;
    for(const pbes_expression& expr: quantified_context)
    {
      assert(is_forall(expr) || is_exists(expr));
      data::variable_list vars(is_forall(expr) ? atermpp::down_cast<forall>(expr).variables() : atermpp::down_cast<exists>(expr).variables());
      for(const data::variable& v: vars)
      {
        qvars.emplace_back(is_forall(expr), v);
      }
    }
    std::pair<qvar_list, propositional_variable_instantiation> QPVI(qvars, x);

    // Store the QPVI and the condition true
    edge_condition ec(data::sort_bool::true_(), data::sort_bool::true_());
    ec.condition.insert(std::make_pair(QPVI, std::set<data::data_expression>{data::sort_bool::true_()}));
    push(ec);
  }

  const condition_map& result() const
  {
    assert(condition_stack.size() == 1);
    return top().condition;
  }
};

} // namespace detail
/// \endcond


/// \brief Algorithm class for the constelm algorithm
template <typename DataRewriter, typename PbesRewriter>
class pbes_constelm_algorithm
{
  protected:
    /// \brief A map with constraints on the vertices of the graph
    typedef std::map<data::variable, data::data_expression> constraint_map;
    typedef std::list<detail::quantified_variable> qvar_list;

    /// \brief Compares data expressions for equality.
    const DataRewriter& m_data_rewriter;

    /// \brief Compares data expressions for equality.
    const PbesRewriter& m_pbes_rewriter;

    /// \brief Represents an edge of the dependency graph. The assignments are stored
    /// implicitly using the 'right' parameter. The condition determines under
    /// what circumstances the influence of the edge is propagated to its target
    /// vertex.
    //
    // N.B. The attribute condition "pbes_expression condition;" needs to be protected.
    // This is achieved by deriving from pbes_expression. This is very ugly, but AFAIK
    // this is the least destructive solution to garbage collection problems.
    // Note that source and target are protected elsewhere.
    class edge: public data::data_expression
    {
      protected:
        /// \brief The propositional variable at the source of the edge
        const propositional_variable m_source;

        qvar_list m_qvars;

        /// \brief The propositional variable instantiation that determines the target of the edge
        const propositional_variable_instantiation m_target;

      public:
        /// \brief Constructor
        edge() = default;

        /// \brief Constructor
        /// \param src A propositional variable declaration
        /// \param tgt A propositional variable
        /// \param c A term
        edge(const propositional_variable& src, const qvar_list& qvars, const propositional_variable_instantiation& tgt, data::data_expression c = data::sort_bool::true_())
          : data::data_expression(c), m_source(src), m_qvars(qvars), m_target(tgt)
        {}

        /// \brief Returns a string representation of the edge.
        /// \return A string representation of the edge.
        std::string to_string() const
        {
          std::ostringstream out;
          out << "(" << m_source.name() << ", " << m_target.name() << ")  label = ";
          for(const detail::quantified_variable& qv: m_qvars)
          {
            out << qv.to_string();
          }
          out << m_target << "  condition = " << condition();
          return out.str();
        }

        /// \brief The propositional variable at the source of the edge
        const propositional_variable& source() const
        {
          return m_source;
        }

        const qvar_list& quantified_variables() const
        {
          return m_qvars;
        }

        /// \brief The propositional variable instantiation that determines the target of the edge
        const propositional_variable_instantiation& target() const
        {
          return m_target;
        }

        /// \brief The condition of the edge
        const data::data_expression& condition() const
        {
          return *this;
        }
    };

    /// \brief Represents a vertex of the dependency graph.
    class vertex
    {
      protected:
        /// \brief The propositional variable that corresponds to the vertex
        propositional_variable m_variable;

        /// \brief The list of quantified variables that occur in the constraints
        qvar_list m_qvars;

        /// \brief Maps data variables to data expressions. If a parameter is not
        // prsent in this map, it means that it represents NaC ("not a constant").
        constraint_map m_constraints;

        /// \brief Indicates whether this vertex has been visited at least once.
        bool m_visited = false;

        /// \brief Returns true if the data variable v has been assigned a constant expression.
        /// \param v A variable
        /// \return True if the data variable v has been assigned a constant expression.
        bool is_constant(const data::variable& v) const
        {
          auto i = m_constraints.find(v);
          return i != m_constraints.end();
        }

        /// \brief Returns true if the expression x has the value undefined_data_expression or if x is a constant data expression.
        /// \param x A
        /// \return True if the data variable v has been assigned a constant expression.
        bool is_constant_expression(const data::data_expression& x) const
        {
          return x == data::undefined_data_expression() || data::is_constant(x);
        }

        template <typename S>
        std::set<data::variable> vars(const S& x)
        {
          return pbes_system::find_free_variables(x);
        }

        bool bound_in_quantifiers(const qvar_list& qvars, const data::data_expression& e)
        {
          std::set<data::variable> free_vars(data::find_free_variables(e));
          return std::all_of(free_vars.begin(), free_vars.end(), [&](const data::variable& v)
            {
              return std::find_if(qvars.begin(), qvars.end(), [&](const detail::quantified_variable& qvar){ return qvar.variable() == v; }) != qvars.end();
            });
        }

        qvar_list project(const qvar_list& qvars, std::set<data::variable> used_variables)
        {
          qvar_list result;
          for(const detail::quantified_variable& v: qvars)
          {
            if(used_variables.find(v.variable()) != used_variables.end())
            {
              result.push_back(v);
            }
          }
          return result;
        }

      public:
        /// \brief Constructor
        vertex() = default;

        /// \brief Constructor
        /// \param x A propositional variable declaration
        vertex(propositional_variable x)
          : m_variable(x)
        {}

        /// \brief The propositional variable that corresponds to the vertex
        const propositional_variable& variable() const
        {
          return m_variable;
        }

        const qvar_list& quantified_variables() const
        {
          return m_qvars;
        }

        /// \brief Maps data variables to data expressions. If the right hand side is a data
        /// variable, it means that it represents NaC ("not a constant").
        const constraint_map& constraints() const
        {
          return m_constraints;
        }

        /// \brief Returns the indices of the constant parameters of this vertex.
        /// \return The indices of the constant parameters of this vertex.
        std::vector<std::size_t> constant_parameter_indices() const
        {
          std::vector<std::size_t> result;
          std::size_t index = 0;
          for (const data::variable& parameter: m_variable.parameters())
          {
            if (is_constant(parameter))
            {
              result.push_back(index);
            }
            index++;
          }
          return result;
        }

        /// \brief Returns a string representation of the vertex.
        /// \return A string representation of the vertex.
        std::string to_string() const
        {
          std::ostringstream out;
          out << m_variable << "  assertions = ";
          for(const detail::quantified_variable& v: quantified_variables())
          {
            out << v.to_string();
          }
          for (const auto& constraint: m_constraints)
          {
            out << "{" << constraint.first << " := " << constraint.second << "} ";
          }
          return out.str();
        }

        /// \brief Assign new values to the parameters of this vertex, and update the constraints accordingly.
        /// The new values have a number of constraints.
        bool update(const qvar_list& qvars, const data::data_expression_list& e, const constraint_map& e_constraints, const DataRewriter& datar)
        {
          bool changed = false;

          data::variable_list params = m_variable.parameters();

          if (!m_visited)
          {
            m_visited = true;
            std::set<data::variable> used_vars;
            auto j = params.begin();
            for (auto i = e.begin(); i != e.end(); ++i, ++j)
            {
              data::rewriter::substitution_type sigma;
              detail::make_constelm_substitution(e_constraints, sigma);
              data::data_expression e1 = datar(*i, sigma);
              if (bound_in_quantifiers(qvars, e1))
              {
                m_constraints[*j] = e1;
                data::find_free_variables(m_constraints[*j], std::inserter(used_vars, used_vars.end()));
              }
            }
            m_qvars = project(qvars, used_vars);
            changed = true;
          }
          else
          {
            auto j = params.begin();
            for (auto i = e.begin(); i != e.end(); ++i, ++j)
            {
              auto k = m_constraints.find(*j);
              if(k == m_constraints.end())
              {
                continue;
              }
              data::data_expression& ci = k->second;
              data::rewriter::substitution_type sigma;
              detail::make_constelm_substitution(e_constraints, sigma);
              data::data_expression ei = datar(*i, sigma);
              if (ci != ei || project(m_qvars, vars(ci)) != project(qvars, vars(ei)))
              {
                m_constraints.erase(k);
                changed = true;
              }
            }
          }
          return changed;
        }
    };

    /// \brief The storage type for vertices
    typedef std::map<core::identifier_string, vertex> vertex_map;

    /// \brief The storage type for edges
    typedef std::map<core::identifier_string, std::vector<edge> > edge_map;

    /// \brief The vertices of the dependency graph. They are stored in a map, to
    /// support searching for a vertex.
    vertex_map m_vertices;

    /// \brief The edges of the dependency graph. They are stored in a map, to
    /// easily access all out-edges corresponding to a particular vertex.
    edge_map m_edges;

    /// \brief The redundant parameters.
    std::map<core::identifier_string, std::vector<std::size_t> > m_redundant_parameters;

    /// \brief Logs the vertices of the dependency graph.
    std::string print_vertices() const
    {
      std::ostringstream out;
      for (const auto& v: m_vertices)
      {
        out << v.second.to_string() << std::endl;
      }
      return out.str();
    }

    /// \brief Logs the edges of the dependency graph.
    std::string print_edges()
    {
      std::ostringstream out;
      for (const auto& source: m_edges)
      {
        for (const auto& e: source.second)
        {
          out << e.to_string() << std::endl;
        }
      }
      return out.str();
    }

    std::string print_todo_list(const std::deque<propositional_variable>& todo)
    {
      std::ostringstream out;
      out << "\n<todo list> [";
      for (auto i = todo.begin(); i != todo.end(); ++i)
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

    std::string print_condition(const edge& e, const vertex& u, const pbes_expression& value)
    {
      std::ostringstream out;
      data::rewriter::substitution_type sigma;
      detail::make_constelm_substitution(u.constraints(), sigma);
      out << "\nEvaluated condition " << e.condition() << sigma << " to " << value << std::endl;
      return out.str();
    }

    std::string print_evaluation_failure(const edge& e, const vertex& u)
    {
      std::ostringstream out;
      data::rewriter::substitution_type sigma;
      detail::make_constelm_substitution(u.constraints(), sigma);
      out << "\nCould not evaluate condition " << e.condition() << sigma << " to true or false";
      return out.str();
    }

    template <typename E>
    std::list<E> concat(const std::list<E> a, const std::list<E> b)
    {
      std::list<E> result(a);
      result.insert(result.end(), b.begin(), b.end());
      return result;
    }

  public:

    /// \brief Constructor.
    /// \param datar A data rewriter
    /// \param pbesr A PBES rewriter
    pbes_constelm_algorithm(const DataRewriter& datar, const PbesRewriter& pbesr)
      : m_data_rewriter(datar), m_pbes_rewriter(pbesr)
    {}

    /// \brief Returns the parameters that have been removed by the constelm algorithm
    /// \return The removed parameters
    std::map<propositional_variable, std::vector<data::variable> > redundant_parameters() const
    {
      std::map<propositional_variable, std::vector<data::variable> > result;
      for (const std::pair<const core::identifier_string, std::vector<std::size_t>>& red_pair: m_redundant_parameters)
      {
        const vertex& v = m_vertices.find(red_pair.first)->second;
        std::vector<data::variable>& variables = result[v.variable()];
        for (const std::size_t par: red_pair.second)
        {
          typename data::variable_list::iterator k = v.variable().parameters().begin();
          std::advance(k, par);
          variables.push_back(*k);
        }
      }
      return result;
    }

    /// \brief Runs the constelm algorithm
    /// \param p A pbes
    /// \param compute_conditions If true, propagation conditions are computed. Note
    /// that the currently implementation has exponential behavior.
    void run(pbes& p, bool compute_conditions = false, bool check_quantifiers = true)
    {
      m_vertices.clear();
      m_edges.clear();
      m_redundant_parameters.clear();

      // compute the vertices and edges of the dependency graph
      for (pbes_equation& eqn: p.equations())
      {
        core::identifier_string name = eqn.variable().name();
        m_vertices[name] = vertex(eqn.variable());

        // use an edge_condition_traverser to compute the edges
        detail::edge_condition_traverser f;
        f.apply(eqn.formula());

        std::vector<edge>& edges = m_edges[name];
        for (auto& [Q_X_e, conditions]: f.result())
        {
          auto& [Q, X_e] = Q_X_e;

          // check options for quantifiers and conditions.
          qvar_list quantifier_list = check_quantifiers ? Q : qvar_list();
          data::data_expression condition = compute_conditions
            ? data::lazy::join_and(conditions.begin(), conditions.end())
            : data::data_expression(data::sort_bool::true_());

          edges.emplace_back(eqn.variable(), quantifier_list, X_e, condition);
        }
      }

      // initialize the todo list of vertices that need to be processed
      propositional_variable_instantiation init = p.initial_state();
      std::deque<propositional_variable> todo;
      const data::data_expression_list& e_init = init.parameters();
      vertex& u_init = m_vertices[init.name()];
      u_init.update(qvar_list(), e_init, constraint_map(), m_data_rewriter);
      todo.push_back(u_init.variable());

      mCRL2log(log::debug) << "\n--- initial vertices ---\n" << print_vertices();
      mCRL2log(log::debug) << "\n--- edges ---\n" << print_edges();

      // propagate constraints over the edges until the todo list is empty
      while (!todo.empty())
      {
        mCRL2log(log::debug) << print_todo_list(todo);
        propositional_variable var = todo.front();

        // remove all occurrences of var from todo
        todo.erase(std::remove(todo.begin(), todo.end(), var), todo.end());

        const vertex& u = m_vertices[var.name()];
        const std::vector<edge>& u_edges = m_edges[var.name()];

        for (const edge& e: u_edges)
        {
          vertex& v = m_vertices[e.target().name()];
          mCRL2log(log::debug) << print_edge_update(e, u, v);

          data::rewriter::substitution_type sigma;
          detail::make_constelm_substitution(u.constraints(), sigma);
          pbes_expression needs_update = m_pbes_rewriter(e.condition(), sigma);
          mCRL2log(log::debug) << print_condition(e, u, needs_update);

          if (!is_false(needs_update) && !is_true(needs_update))
          {
            mCRL2log(log::debug) << print_evaluation_failure(e, u);
          }
          if (!is_false(needs_update))
          {
            bool changed = v.update(concat(v.quantified_variables(), e.quantified_variables()), e.target().parameters(), u.constraints(), m_data_rewriter);
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
      for (const pbes_equation& eqn: p.equations())
      {
        core::identifier_string name = eqn.variable().name();
        const vertex& v = m_vertices[name];
        if (!v.constraints().empty())
        {
          std::vector<std::size_t> r = v.constant_parameter_indices();
          if (!r.empty())
          {
            m_redundant_parameters[name] = r;
          }
        }
      }

      // Apply the constraints to the equations.
      for (pbes_equation& eqn: p.equations())
      {
        core::identifier_string name = eqn.variable().name();
        const vertex& v = m_vertices[name];

        if (!v.constraints().empty())
        {
          data::rewriter::substitution_type sigma;
          detail::make_constelm_substitution(v.constraints(), sigma);
          pbes_expression body = pbes_system::replace_free_variables(eqn.formula(), sigma);
          for (auto i = v.quantified_variables().crbegin(); i != v.quantified_variables().crend(); ++i)
          {
            body = i->make_expr(body);
          }
          eqn = pbes_equation(
                   eqn.symbol(),
                   eqn.variable(),
                   body
                 );
        }
      }

      // remove the redundant parameters
      pbes_system::algorithms::remove_parameters(p, m_redundant_parameters);

      // print the parameters and equation that are removed
      if (mCRL2logEnabled(log::verbose))
      {
        mCRL2log(log::verbose) << "\nremoved the following constant parameters:" << std::endl;
        for (const std::pair<const propositional_variable, std::vector<data::variable>>& i: redundant_parameters())
        {
          for (const data::variable& var: i.second)
          {
            mCRL2log(log::verbose) << "  (" << mcrl2::core::pp(i.first.name()) << ", " << data::pp(var) << ")" << std::endl;
          }
        }
      }
    }
};

/// \brief Apply the constelm algorithm
/// \param p A PBES to which the algorithm is applied
/// \param rewrite_strategy A data rewrite strategy
/// \param rewriter_type A PBES rewriter type
/// \param compute_conditions If true, conditions for the edges of the dependency graph are used N.B. Very inefficient!
/// \param remove_redundant_equations If true, unreachable equations will be removed.
inline
void constelm(pbes& p,
              data::rewrite_strategy rewrite_strategy,
              pbes_rewriter_type rewriter_type,
              bool compute_conditions = false,
              bool remove_redundant_equations = true,
              bool check_quantifiers = true
             )
{
  // data rewriter
  data::rewriter datar(p.data(), rewrite_strategy);

  // pbes rewriter
  switch (rewriter_type)
  {
    case simplify:
    {
      typedef simplify_data_rewriter<data::rewriter> pbes_rewriter;
      pbes_rewriter pbesr(datar);
      pbes_constelm_algorithm<data::rewriter, pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, compute_conditions, check_quantifiers);
      if (remove_redundant_equations)
      {
        std::vector<propositional_variable> V = algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << algorithms::print_removed_equations(V);
      }
      break;
    }
    case quantifier_all:
    case quantifier_finite:
    {
      bool enumerate_infinite_sorts = (rewriter_type == quantifier_all);
      enumerate_quantifiers_rewriter pbesr(datar, p.data(), enumerate_infinite_sorts);
      pbes_constelm_algorithm<data::rewriter, enumerate_quantifiers_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, compute_conditions, check_quantifiers);
      if (remove_redundant_equations)
      {
        std::vector<propositional_variable> V = algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << algorithms::print_removed_equations(V);
      }
      break;
    }
    default:
    { }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_CONSTELM_H
