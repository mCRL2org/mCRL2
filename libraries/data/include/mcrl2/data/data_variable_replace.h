// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_variable_replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DATA_VARIABLE_REPLACE_H
#define MCRL2_DATA_DATA_VARIABLE_REPLACE_H

#include <map>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
struct data_variable_map_replace_helper
{
  const std::map<data_variable, data_expression>& m_replacements;
    
  /// Constructor.
  ///
  data_variable_map_replace_helper(const std::map<data_variable, data_expression>& replacements)
    : m_replacements(replacements)
  {}
  
  /// Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  ///
  data_expression operator()(const data_variable& t) const
  {
    std::map<data_variable, data_expression>::const_iterator i = m_replacements.find(t);
    if (i == m_replacements.end())
    {
      return aterm_appl(t);
    }
    else
    {
      return i->second;
    }
  }
};
/// \endcond

/// Replaces all data_variables in the term t using the specified map of replacements.
template <typename Term>
Term data_variable_map_replace(Term t, const std::map<data_variable, data_expression>& replacements)
{
  return atermpp::checked_replace(t, is_data_variable, data_variable_map_replace_helper(replacements));
}

/// \cond INTERNAL_DOCS
template <typename ReplaceFunction>
struct replace_data_variables_helper
{
  const ReplaceFunction& r_;
  
  replace_data_variables_helper(const ReplaceFunction& r)
    : r_(r)
  {}
  
  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (is_data_variable(t))
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
/// each data variable that is encountered during the traversal.
template <typename Term, typename ReplaceFunction>
Term replace_data_variables(Term t, ReplaceFunction r)
{
  return atermpp::partial_replace(t, replace_data_variables_helper<ReplaceFunction>(r)); 
}

/// \cond INTERNAL_DOCS
template <typename VariableContainer, typename ExpressionContainer>
struct replace_data_variable_sequence_helper
{
  const VariableContainer& variables_;
  const ExpressionContainer& replacements_;
  
  replace_data_variable_sequence_helper(const VariableContainer& variables,
                                        const ExpressionContainer& replacements
                                       )
    : variables_(variables), replacements_(replacements)
  {
    assert(variables.size() == replacements.size());
  }
  
  data_expression operator()(data_variable t) const
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
replace_data_variable_sequence_helper<V, E>
make_replace_data_variable_sequence_helper(const V& variables,
                                           const E& replacements
                                          )
{
  return replace_data_variable_sequence_helper<V, E>(variables, replacements);
}
/// \endcond

/// Replaces all data_variables in the term t using the specified sequence of replacements.
/// \param variables The sequence of variables that need to be replaced.
/// \param replacements The corresponding replacements.
template <typename Term, typename VariableContainer, typename ExpressionContainer>
Term replace_data_variable_sequence(Term t,
                                    const VariableContainer& variables,
                                    const ExpressionContainer& replacements
                                   )
{
  return replace_data_variables(t, make_replace_data_variable_sequence_helper(variables, replacements));
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_VARIABLE_REPLACE_H
