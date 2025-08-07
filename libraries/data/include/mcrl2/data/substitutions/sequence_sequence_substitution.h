// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/sequence_sequence_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_SEQUENCE_SEQUENCE_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_SEQUENCE_SEQUENCE_SUBSTITUTION_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2::data
{

/// \brief Generic substitution function. The substitution is stored as a sequence
/// of variables and a sequence of expressions.
template <typename VariableContainer, typename ExpressionContainer>
struct sequence_sequence_substitution
{
  /// \brief type used to represent variables
  using variable_type = typename VariableContainer::value_type;

  /// \brief type used to represent expressions
  using expression_type = typename ExpressionContainer::value_type;

  using argument_type = variable_type;
  using result_type = expression_type;

  const VariableContainer& variables;
  const ExpressionContainer& expressions;

  sequence_sequence_substitution(const VariableContainer& variables_, const ExpressionContainer& expressions_)
    : variables(variables_),
      expressions(expressions_)
  {
    assert(variables.size() == expressions.size());
  }

  expression_type operator()(const variable_type& v) const
  {
    typename VariableContainer::const_iterator i = variables.begin();
    typename ExpressionContainer::const_iterator j = expressions.begin();

    for (; i != variables.end(); ++i, ++j)
    {
      if (*i == v)
      {
        return *j;
      }
    }
    return expression_type(v);
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    typename VariableContainer::const_iterator i = variables.begin();
    typename ExpressionContainer::const_iterator j = expressions.begin();
    for (; i != variables.end(); ++i, ++j)
    {
      out << (i == variables.begin() ? "" : "; ") << *i << " := " << *j;
    }
    out << "]";
    return out.str();
  }
};

/// \brief Utility function for creating a sequence_sequence_substitution.
template <typename VariableContainer, typename ExpressionContainer>
sequence_sequence_substitution<VariableContainer, ExpressionContainer>
make_sequence_sequence_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return sequence_sequence_substitution<VariableContainer, ExpressionContainer>(vc, ec);
}

template <typename VariableContainer, typename ExpressionContainer>
std::ostream& operator<<(std::ostream& out, const sequence_sequence_substitution<VariableContainer, ExpressionContainer>& sigma)
{
  return out << sigma.to_string();
}

template <typename VariableContainer, typename ExpressionContainer>
bool is_simple_substitution(const sequence_sequence_substitution<VariableContainer, ExpressionContainer>& sigma)
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

#endif // MCRL2_DATA_SUBSTITUTIONS_SEQUENCE_SEQUENCE_SUBSTITUTION_H
