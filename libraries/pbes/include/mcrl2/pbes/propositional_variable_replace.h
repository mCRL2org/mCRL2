// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/propositional_variable_replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_PBES_PROPOSITIONAL_VARIABLE_REPLACE_H
#define MCRL2_PBES_PROPOSITIONAL_VARIABLE_REPLACE_H

#include <map>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data_variable_replace.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
template <typename ReplaceFunction>
struct replace_propositional_variables_helper
{
  const ReplaceFunction& r_;
  
  replace_propositional_variables_helper(const ReplaceFunction& r)
    : r_(r)
  {}
  
  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (is_propositional_variable_instantiation(t))
    {
      return std::pair<aterm_appl, bool>(r_(t), false); // do not continue the recursion
    }
    else
    {
      return std::pair<aterm_appl, bool>(t, true); // continue the recursion
    }
  }
};
/// \endcond

/// Recursively traverses the given term, and applies the replace function to
/// each propositional variable instantiation that is encountered during the
/// traversal.
template <typename Term, typename ReplaceFunction>
Term replace_propositional_variables(Term t, ReplaceFunction r)
{
  return atermpp::partial_replace(t, replace_propositional_variables_helper<ReplaceFunction>(r)); 
}

/// \cond INTERNAL_DOCS
template <typename VariableContainer, typename ExpressionContainer>
struct replace_propositional_variable_sequence_helper
{
  const VariableContainer& variables_;
  const ExpressionContainer& replacements_;
  
  replace_propositional_variable_sequence_helper(const VariableContainer& variables,
                                        const ExpressionContainer& replacements
                                       )
    : variables_(variables), replacements_(replacements)
  {
    assert(variables.size() == replacements.size());
  }
  
  pbes_expression operator()(propositional_variable_instantiation t) const
  {
    typename VariableContainer::const_iterator i = variables_.begin();
    typename ExpressionContainer::const_iterator j = replacements_.begin();
    for (; i != variables_.end(); ++i, ++j)
    {
      if (*i == t)
      {
        return *j;
      }
    }
    return t;
  }
};

template <typename V, typename E>
replace_propositional_variable_sequence_helper<V, E>
make_replace_propositional_variable_sequence_helper(const V& variables,
                                           const E& replacements
                                          )
{
  return replace_propositional_variable_sequence_helper<V, E>(variables, replacements);
}
/// \endcond

/// Replaces all propositional_variables in the term t using the specified sequence of replacements.
/// \param variables The sequence of variables that need to be replaced.
/// \param replacements The corresponding replacements.
template <typename Term, typename VariableContainer, typename ExpressionContainer>
Term replace_propositional_variable_sequence(Term t,
                                    const VariableContainer& variables,
                                    const ExpressionContainer& replacements
                                   )
{
  return replace_propositional_variables(t, make_replace_propositional_variable_sequence_helper(variables, replacements));
}

/// \cond INTERNAL_DOCS
struct substitute_propositional_variable_helper
{
  const propositional_variable& variable_;
  const pbes_expression& replacement_;
  
  substitute_propositional_variable_helper(const propositional_variable& variable,
                                           const pbes_expression& replacement
                                          )
    : variable_(variable), replacement_(replacement)
  { }
  
  pbes_expression operator()(propositional_variable_instantiation t) const
  {
    if (variable_.name() != t.name())
    {
      return t;
    }
    else
    {
      return data::replace_data_variable_sequence(replacement_, variable_.parameters(), t.parameters());
    }
  }
};
/// \endcond

/// Applies the substitution X := phi to the pbes expression t.
/// \return The result of the substitution.
inline
pbes_expression substitute_propositional_variable(pbes_expression t,
                                                  const propositional_variable& X,
                                                  const pbes_expression& phi)
{
  return replace_propositional_variables(t, substitute_propositional_variable_helper(X, phi)); 
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PROPOSITIONAL_VARIABLE_REPLACE_H
