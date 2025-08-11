// Author(s): Muck van Weerdenburg, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/enumerator_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/is_simple_substitution.h"

namespace mcrl2::data
{

namespace detail {

// applies the enumerator substitution defined by variables and expressions to x
template<typename T>
data_expression enumerator_replace(const T& x,
  variable_list::const_iterator variables_begin,
  variable_list::const_iterator variables_end,
  data_expression_list::const_iterator expressions_begin);

struct enumerator_replace_builder: public data_expression_builder<enumerator_replace_builder>
{
  using super = data_expression_builder<enumerator_replace_builder>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const variable_list::const_iterator m_vars_begin;
  const variable_list::const_iterator m_vars_end;
  const data_expression_list::const_iterator m_expressions_begin;

  enumerator_replace_builder(const variable_list::const_iterator variables_begin, 
                             const variable_list::const_iterator variables_end, 
                             const data_expression_list::const_iterator expressions_begin)
    : m_vars_begin(variables_begin),
      m_vars_end(variables_end),
      m_expressions_begin(expressions_begin)
  {}

  template <class T>
  void apply(T& result, const variable& x)
  {
    variable_list::const_iterator i_vars = m_vars_begin;
    data_expression_list::const_iterator i_exprs = m_expressions_begin;
    while (i_vars!=m_vars_end && x != *i_vars)
    {
      ++i_vars;
      ++i_exprs;
    }
    if (i_vars==m_vars_end)
    {
      result = x;
      return;
    }
    else
    {
      result = enumerator_replace(*i_exprs, i_vars, m_vars_end, i_exprs);
      return;
    }
  }
};

template <typename T>
inline
data_expression enumerator_replace(const T& x, 
                                   const variable_list::const_iterator variables_begin, 
                                   const variable_list::const_iterator variables_end,  
                                   const data_expression_list::const_iterator expressions_begin)
{
  data_expression result;
  enumerator_replace_builder f(variables_begin, variables_end, expressions_begin);
  f.apply(result, x);
  return result;
}

template <typename T>
inline
data_expression enumerator_replace(const T& x, const variable_list& variables, const data_expression_list& expressions)
{
  assert(variables.size()==expressions.size());
  return enumerator_replace(x, variables.begin(), variables.end(), expressions.begin());
}

} // namespace detail

/// \brief Substitution that stores the assignments as a sequence of variables and a sequence of expressions.
/// It supports function composition efficiently. This is done by simply concatenating the variables and
/// expressions of the two substitutions. As a result, evaluating the substitution becomes more expensive.
struct enumerator_substitution
{
  /// \brief type used to represent variables
  using variable_type = data::variable;

  /// \brief type used to represent expressions
  using expression_type = data::data_expression;

  data::variable_list variables;
  data::data_expression_list expressions;

  enumerator_substitution() = default;

  enumerator_substitution(data::variable_list variables_, data::data_expression_list expressions_)
    : variables(std::move(variables_)),
      expressions(std::move(expressions_))
  {
    assert(variables.size() == expressions.size());
  }

  data::data_expression operator()(const data::variable& v) const
  {
    return detail::enumerator_replace(v, variables, expressions);
  }

  // Adds the assignment [v := e] to this substitution, by putting it in front of the lists with variables and expressions.
  // Note that this operation has not the same effect as function composition with [v := e]. Therefore we use a different
  // syntax than sigma[v] = e.
  void add_assignment(const data::variable& v, const data::data_expression& e)
  {
    variables.push_front(v);
    expressions.push_front(e);
  }

  // Reverses the order of the assignments in the substitution.
  void revert()
  {
    variables = atermpp::reverse(variables);
    expressions = atermpp::reverse(expressions);
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

inline
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

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_ENUMERATOR_SUBSTITUTION_H
