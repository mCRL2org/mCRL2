// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/find.h
/// \brief add your file description here.

#ifndef MCRL2_BES_FIND_H
#define MCRL2_BES_FIND_H

#include <iterator>
#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/bes/traverser.h"
#include "mcrl2/core/detail/find.h"

namespace mcrl2 {

namespace bes {

  /// \brief Returns all boolean variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all variables occurring in t
  ///             are added.
  /// \return All variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_variables(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<boolean_variable, bes::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All variables that occur in the term t
  template <typename Container>
  std::set<boolean_variable> find_variables(Container const& container)
  {
    std::set<boolean_variable> result;
    bes::find_variables(container, std::inserter(result, result.end()));
    return result;
  }

  // TODO: this is a temporary solution
  struct compare_boolean_variable
  {
    const boolean_variable& m_variable;
  
    compare_boolean_variable(const boolean_variable& t)
     : m_variable(t)
    {}
  
    /// \brief Function call operator
    /// \param t A term
    /// \return The function result
    template <typename Variable>
    bool operator()(const Variable& t) const
    {
      return m_variable == t;
    }
  };

  /// \brief Returns true if the term has a given boolean variable as subterm.
  /// \param[in] container an expression or container with expressions
  /// \param[in] v an expression or container with expressions
  /// \param d A boolean variable
  /// \return True if the term has a given boolean variable as subterm.
  template <typename Container>
  bool search_variable(Container const& container, const boolean_variable& v)
  {
    return core::detail::make_search_helper<boolean_variable, bes::selective_traverser>(compare_boolean_variable(v)).apply(container);
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_FIND_H
