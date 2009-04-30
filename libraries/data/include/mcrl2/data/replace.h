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
#include "boost/utility/enable_if.hpp"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/detail/concepts.h"

namespace mcrl2 {

namespace data {

//----------------------------------------------------------------------------------------//
//                                data variable stuff
//----------------------------------------------------------------------------------------//

/// \cond INTERNAL_DOCS
template <typename ReplaceFunction>
struct replace_variables_helper // TODO make proper visitor on structure of data expression
{
  const ReplaceFunction& r_;

  mutable std::set< variable > m_bound;

  replace_variables_helper(const ReplaceFunction& r)
    : r_(r)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (data_expression(t).is_abstraction())
    {
      abstraction::variables_const_range bound(abstraction(t).variables());

      m_bound.insert(bound.begin(), bound.end());
    }
    else if (data_expression(t).is_variable())
    {
      return
        std::pair<atermpp::aterm_appl, bool>(
             (m_bound.find(t) != m_bound.end()) ? t : r_(static_cast< variable >(t)), false);
    }

    return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
  }
};
/// \endcond

/// \cond INTERNAL_DOCS
namespace detail {

  // The last argument of the functions below is used with boost::enable_if to
  // activate the correct overload. This way the compiler generates the
  // necessary code, which would otherwise need to be duplicated *at least*
  // four times for frequently used containers (currently std::set,
  // atermpp::set, std::vector, atermpp::vector). Adding overloads for a new
  // container type only requires instantiation of is_container_impl for the
  // appropriate type.

  template <typename T, typename ReplaceFunction >
  T partial_replace(T t, ReplaceFunction r, typename boost::disable_if< typename is_container< T >::type >::type* = 0)
  {
    return atermpp::partial_replace(t, r);
  }

  template <typename T, typename ReplaceFunction >
  T partial_replace(T const& t, ReplaceFunction r, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    T result;

    typename std::insert_iterator< T > j(result, result.end());

    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i) {
      *j = atermpp::partial_replace(*i, r);
    }

    return result;
  }

  template <typename T, typename ReplaceFunction >
  T& partial_replace(T& t, ReplaceFunction r, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    for (typename T::iterator i = t.begin(); i != t.end(); ++i) {
      *i = atermpp::partial_replace(*i, r);
    }

    return t;
  }
}
/// \endcond

/// \brief Recursively traverses the given term, and applies the replace function to
/// each data variable that is encountered during the traversal.
/// \param t A term
/// \param r A replace function
/// \return The replacement result
template <typename Term, typename ReplaceFunction>
Term replace_variables(Term t, ReplaceFunction r)
{
  BOOST_CONCEPT_ASSERT((concepts::Substitution<ReplaceFunction>));
  return detail::partial_replace(t, replace_variables_helper<ReplaceFunction>(r));
}

/// \cond INTERNAL_DOCS
template <typename VariableContainer, typename ExpressionContainer>
struct variable_sequence_replace_helper: public core::substitution_function<typename VariableContainer::value_type, typename ExpressionContainer::value_type>
{
  const VariableContainer& variables_;
  const ExpressionContainer& replacements_;

  variable_sequence_replace_helper(const VariableContainer& variables,
                                   const ExpressionContainer& replacements
                                  )
    : variables_(variables), replacements_(replacements)
  {
    assert(variables.size() == replacements.size());
  }

  /// \brief Function call operator
  /// \param t A data variable
  /// \return The function result
  data_expression operator()(variable t) const
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
Term variable_sequence_replace(Term t,
                               const VariableContainer& variables,
                               const ExpressionContainer& replacements
                              )
{
  return replace_variables(t, variable_sequence_replace_helper<VariableContainer, ExpressionContainer>(variables, replacements));
}

/// \cond INTERNAL_DOCS
template <typename MapContainer>
struct variable_map_replace_helper: public core::substitution_function<typename MapContainer::key_type, typename MapContainer::mapped_type>
{
  const MapContainer& replacements_;

  /// \brief Constructor.
  /// \param replacements A mapping of data variable replacements
  variable_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  /// \param t A data variable
  /// \return The function result
  data_expression operator()(const variable& t) const
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

/// \brief Replaces all variables in the term t using the specified map of replacements.
/// \param t A term
/// \param replacements A map of replacements
/// \return The replacement result. Each variable \p v in t that occurs as key in the map
/// \p replacements is replaced by \p replacements[\p v].
template <typename Term, typename MapContainer>
Term variable_map_replace(Term t, const MapContainer& replacements)
{
  return replace_variables(t, variable_map_replace_helper<MapContainer>(replacements));
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

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
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
Term replace_data_expressions(Term const t, ReplaceFunction r)
{
  BOOST_CONCEPT_ASSERT((boost::UnaryFunction<ReplaceFunction, data_expression, data_expression>));
  return detail::partial_replace(t, replace_data_expressions_helper<ReplaceFunction>(r));
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

  /// \brief Function call operator
  /// \param t A data expression
  /// \return The function result
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
  /// \param replacements A map of data variable replacements
  data_expression_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  /// \param t A data expression
  /// \return The function result
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
