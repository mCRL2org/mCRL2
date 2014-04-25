// Author(s): Muck van Weerdenburg, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/enumerator_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

namespace detail {

data_expression build_solution_aux(const data_expression& t, const variable_list& substituted_vars, const data_expression_list& exprs);

inline
data_expression build_solution_single(const variable& t, variable_list substituted_vars, data_expression_list exprs)
{
  assert(substituted_vars.size() == exprs.size());
  while (!substituted_vars.empty() && t != substituted_vars.front())
  {
    substituted_vars.pop_front();
    exprs.pop_front();
  }
  if (substituted_vars.empty())
  {
    return t;
  }
  else
  {
    return build_solution_aux(exprs.front(), substituted_vars.tail(), exprs.tail());
  }
}

class apply_build_solution_aux
{
  protected:
    const variable_list& m_substituted_vars;
    const data_expression_list& m_expr;

  public:
    apply_build_solution_aux(const variable_list& substituted_vars, const data_expression_list& expr):
       m_substituted_vars(substituted_vars), m_expr(expr)
    {}

    data_expression operator()(const data_expression& t) const
    {
      return build_solution_aux(t, m_substituted_vars, m_expr);
    }
};

data_expression build_solution_aux(const data_expression& t, const variable_list& substituted_vars, const data_expression_list& exprs)
{
  assert(!is_where_clause(t)); // This is a non expected case as t is a normalform.
  if (is_variable(t))
  {
    return build_solution_single(atermpp::aterm_cast<variable>(t), substituted_vars, exprs);
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1 = core::down_cast<abstraction>(t);
    const binder_type& binder = t1.binding_operator();
    const variable_list& bound_variables = t1.variables();
    const data_expression& body = build_solution_aux(t1.body(), substituted_vars, exprs);
    return abstraction(binder, bound_variables, body);
  }
  else if (is_function_symbol(t))
  {
    return t;
  }

  assert(is_application(t));
  {
    // t has the shape application(u1,...,un)
    const application t_appl(t);
    const data_expression& head = t_appl.head();

    if (is_function_symbol(head))
    {
      return application(head, t_appl.begin(), t_appl.end(), apply_build_solution_aux(substituted_vars, exprs));
    }

    /* The head is more complex, rewrite it first; */

    data_expression head1 = build_solution_aux(head, substituted_vars, exprs);
    return application(head1, t_appl.begin(), t_appl.end(), apply_build_solution_aux(substituted_vars, exprs));
  }
}

} // namespace detail

/// \brief Substitution that stores the assignments as a sequence of variables and a sequence of expressions.
/// It supports function composition efficiently. This is done by simply concatenating the variables and
/// expressions of the two substitutions. As a result, evaluating the substitution becomes more expensive.
struct enumerator_substitution: public std::unary_function<data::variable, data::data_expression>
{
  /// \brief type used to represent variables
  typedef data::variable variable_type;

  /// \brief type used to represent expressions
  typedef data::data_expression expression_type;

  data::variable_list variables;
  data::data_expression_list expressions;

  /// \brief Wrapper class for internal storage and substitution updates using operator()
  struct assignment
  {
    data::variable m_variable;
    data::variable_list& m_variables;
    data::data_expression_list& m_expressions;

    /// \brief Constructor.
    ///
    /// \param[in] v a variable.
    assignment(const data::variable& v, data::variable_list& variables, data::data_expression_list& expressions)
      : m_variable(v), m_variables(variables), m_expressions(expressions)
    { }

    template <typename AssignableToExpression>
    void operator=(AssignableToExpression const& e)
    {
      mCRL2log(log::debug2, "substitutions") << "Setting " << m_variable << " := " << e << std::endl;
      m_variables.push_front(m_variable);
      m_expressions.push_front(e);
    }
  };

  enumerator_substitution()
  {}

  enumerator_substitution(const data::variable_list& variables_, const data::data_expression_list& expressions_)
    : variables(variables_),
      expressions(expressions_)
  {
    assert(variables.size() == expressions.size());
  }

  data::data_expression operator()(const data::variable& v) const
  {
    return detail::build_solution_single(v, variables, expressions);
  }

  assignment operator[](const data::variable& v)
  {
    return assignment(v, variables, expressions);
  }

  template <typename Expression>
  data::data_expression operator()(const Expression&) const
  {
    throw mcrl2::runtime_error("data::enumerator_substitution::operator(const Expression&) is a deprecated interface!");
    return data::undefined_data_expression();
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    auto i = variables.begin();
    auto j = expressions.begin();
    for (; i != variables.end(); ++i, ++j)
    {
      out << (i == variables.begin() ? "" : "; ") << *i << " := " << *j;
    }
    out << "]";
    return out.str();
  }
};

inline
std::ostream& operator<<(std::ostream& out, const enumerator_substitution& sigma)
{
  return out << sigma.to_string();
}

bool is_simple_substitution(const enumerator_substitution& sigma)
{
  auto i = sigma.variables.begin();
  auto j = sigma.expressions.begin();
  for (i = sigma.variables.begin(); i != sigma.variables.end(); ++i, ++j)
  {
    if (!is_simple_substitution(*i, *j))
    {
      return false;
    }
  }
  return true;
}

// returns the function composition f o g
inline
enumerator_substitution compose(const enumerator_substitution& f, const enumerator_substitution& g)
{
  // N.B. TODO: this can probably be done more efficiently
  return enumerator_substitution(g.variables + f.variables, g.expressions + f.expressions);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H
