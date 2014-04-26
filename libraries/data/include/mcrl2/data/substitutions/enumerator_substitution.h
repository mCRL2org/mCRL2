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
#include "mcrl2/data/builder.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

namespace detail {

// applies the enumerator substitution defined by variables and expressions to x
template <typename T>
data_expression enumerator_replace(const T& x, const variable_list& variables, const data_expression_list& expressions);

struct enumerator_replace_builder: public data_expression_builder<enumerator_replace_builder>
{
  typedef data_expression_builder<enumerator_replace_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

  const variable_list& variables;
  const data_expression_list& expressions;

  enumerator_replace_builder(const variable_list& variables_, const data_expression_list& expressions_)
    : variables(variables_),
      expressions(expressions_)
  {}

  data_expression operator()(const variable& x)
  {
    variable_list vars = variables;
    data_expression_list exprs = expressions;
    while (!vars.empty() && x != vars.front())
    {
      vars.pop_front();
      exprs.pop_front();
    }
    if (vars.empty())
    {
      return x;
    }
    else
    {
      return enumerator_replace(exprs.front(), vars.tail(), exprs.tail());
    }
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <typename T>
inline
data_expression enumerator_replace(const T& x, const variable_list& variables, const data_expression_list& expressions)
{
  enumerator_replace_builder f(variables, expressions);
  return f(x);
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
    return detail::enumerator_replace(v, variables, expressions);
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
