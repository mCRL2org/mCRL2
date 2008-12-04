// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_PBES_REPLACE_H
#define MCRL2_PBES_REPLACE_H

#include <utility>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/replace.h"
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

  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (is_propositional_variable_instantiation(t))
    {
      return std::pair<atermpp::aterm_appl, bool>(r_(t), false); // do not continue the recursion
    }
    else
    {
      return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
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
struct propositional_variable_sequence_replace_helper
{
  const VariableContainer& variables_;
  const ExpressionContainer& replacements_;

  propositional_variable_sequence_replace_helper(const VariableContainer& variables,
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
/// \endcond

/// Replaces all propositional_variables in the term t using the specified sequence of replacements.
/// \param variables The sequence of variables that need to be replaced.
/// \param replacements The corresponding replacements.
template <typename Term, typename VariableContainer, typename ExpressionContainer>
Term propositional_variable_sequence_replace(Term t,
                                    const VariableContainer& variables,
                                    const ExpressionContainer& replacements
                                   )
{
  return replace_propositional_variables(t, propositional_variable_sequence_replace_helper<VariableContainer, ExpressionContainer>(variables, replacements));
}

/// \cond INTERNAL_DOCS
template <typename MapContainer>
struct propositional_variable_map_replace_helper
{
  const MapContainer& replacements_;

  /// \brief Constructor.
  propositional_variable_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  pbes_expression operator()(const propositional_variable_instantiation& t) const
  {
    typename MapContainer::const_iterator i = replacements_.find(t);
    if (i == replacements_.end())
    {
      return atermpp::aterm_appl(t);
    }
    else
    {
      return i->second;
    }
  }
};
/// \endcond

/// Replaces all propositional_variable_instantiations in the term t using the specified map of replacements.
template <typename Term, typename MapContainer>
Term propositional_variable_map_replace(Term t, const MapContainer& replacements)
{
  return replace_propositional_variable_instantiations(t, propositional_variable_map_replace_helper<MapContainer>(replacements));
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
      return data::data_variable_sequence_replace(replacement_, variable_.parameters(), t.parameters());
    }
  }
};
/// \endcond

/// \brief Applies the substitution X := phi to the pbes expression t.
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

#endif // MCRL2_PBES_REPLACE_H
