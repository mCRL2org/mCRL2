// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_REPLACE_H
#define MCRL2_DATA_REPLACE_H

#include <utility>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace data {

//----------------------------------------------------------------------------------------//
//                                data variable stuff
//----------------------------------------------------------------------------------------//

/// \cond INTERNAL_DOCS
template <typename ReplaceFunction>
struct replace_data_variables_helper
{
  const ReplaceFunction& r_;

  replace_data_variables_helper(const ReplaceFunction& r)
    : r_(r)
  {}

  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (is_data_variable(t))
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

/// \brief Recursively traverses the given term, and applies the replace function to
/// each data variable that is encountered during the traversal.
/// \param t A term
/// \param r A replace function
/// \return The replacement result
template <typename Term, typename ReplaceFunction>
Term replace_data_variables(Term t, ReplaceFunction r)
{
  return atermpp::partial_replace(t, replace_data_variables_helper<ReplaceFunction>(r));
}

/// \cond INTERNAL_DOCS
template <typename VariableContainer, typename ExpressionContainer>
struct data_variable_sequence_replace_helper
{
  const VariableContainer& variables_;
  const ExpressionContainer& replacements_;

  data_variable_sequence_replace_helper(const VariableContainer& variables,
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
/// \endcond

/// \brief Replaces variables in the term t using the specified sequence of replacements.
/// \param t A term
/// \param variables A sequence of variables
/// \param replacements A sequence of expressions
/// \return The replacement result. Each variable in \p t that occurs as the i-th element
/// of variables is replaced by the i-th element of \p expressions. If the sequence
/// \p variables contains duplicates, the first match is selected.
template <typename Term, typename VariableContainer, typename ExpressionContainer>
Term data_variable_sequence_replace(Term t,
                                    const VariableContainer& variables,
                                    const ExpressionContainer& replacements
                                   )
{
  return replace_data_variables(t, data_variable_sequence_replace_helper<VariableContainer, ExpressionContainer>(variables, replacements));
}

/// \cond INTERNAL_DOCS
template <typename MapContainer>
struct data_variable_map_replace_helper
{
  const MapContainer& replacements_;

  /// \brief Constructor.
  ///
  data_variable_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  ///
  data_expression operator()(const data_variable& t) const
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

/// \brief Replaces all data_variables in the term t using the specified map of replacements.
/// \param t A term
/// \param replacements A map of replacements
/// \return The replacement result. Each variable \p v in t that occurs as key in the map
/// \p replacements is replaced by \p replacements[\p v].
template <typename Term, typename MapContainer>
Term data_variable_map_replace(Term t, const MapContainer& replacements)
{
  return replace_data_variables(t, data_variable_map_replace_helper<MapContainer>(replacements));
}

//----------------------------------------------------------------------------------------//
//                                data expression stuff
//----------------------------------------------------------------------------------------//

/// \cond INTERNAL_DOCS
template <typename ReplaceFunction>
struct replace_data_expressions_helper
{
  const ReplaceFunction& r_;

  replace_data_expressions_helper(const ReplaceFunction& r)
    : r_(r)
  {}

  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (is_data_expression(t))
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

/// \brief Recursively traverses the given term, and applies the replace function to
/// each data expression that is encountered during the traversal.
/// \param t A term
/// \param r A replace function
/// \return The replacement result
template <typename Term, typename ReplaceFunction>
Term replace_data_expressions(Term t, ReplaceFunction r)
{
  return atermpp::partial_replace(t, replace_data_expressions_helper<ReplaceFunction>(r));
}

/// \cond INTERNAL_DOCS
template <typename VariableContainer, typename ExpressionContainer>
struct data_expression_sequence_replace_helper
{
  const VariableContainer& expressions_;
  const ExpressionContainer& replacements_;

  data_expression_sequence_replace_helper(const VariableContainer& expressions,
                                        const ExpressionContainer& replacements
                                       )
    : expressions_(expressions), replacements_(replacements)
  {
    assert(expressions.size() == replacements.size());
  }

  data_expression operator()(data_expression t) const
  {
    typename VariableContainer::const_iterator i = expressions_.begin();
    typename ExpressionContainer::const_iterator j = replacements_.begin();
    for (; i != expressions_.end(); ++i, ++j)
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

/// \brief Replaces data expressions in the term t using the specified sequence of replacements.
/// \param t A term
/// \param expressions A sequence of data expressions
/// \param replacements A sequence of data expressions
/// \return The replacement result. Each data expression in \p t that occurs as the i-th element
/// of \p expressions is replaced by the i-th element of \p replacements. If the sequence
/// \p expressions contains duplicates, the first match is selected.
template <typename Term, typename VariableContainer, typename ExpressionContainer>
Term data_expression_sequence_replace(Term t,
                                    const VariableContainer& expressions,
                                    const ExpressionContainer& replacements
                                   )
{
  return replace_data_expressions(t, data_expression_sequence_replace_helper<VariableContainer, ExpressionContainer>(expressions, replacements));
}

/// \cond INTERNAL_DOCS
template <typename MapContainer>
struct data_expression_map_replace_helper
{
  const MapContainer& replacements_;

  /// \brief Constructor.
  ///
  data_expression_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  ///
  data_expression operator()(const data_expression& t) const
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

/// \brief Replaces all data_expressions in the term t using the specified map of replacements.
/// \param t A term
/// \param replacements A map of replacements
/// \return The replacement result. Each data expression \p e in t that occurs as key in the map
/// \p replacements is replaced by \p replacements[\p e].
template <typename Term, typename MapContainer>
Term data_expression_map_replace(Term t, const MapContainer& replacements)
{
  return replace_data_expressions(t, data_expression_map_replace_helper<MapContainer>(replacements));
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_H
