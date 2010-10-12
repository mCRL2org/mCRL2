// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/new_find.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_NEW_FIND_H
#define MCRL2_DATA_NEW_FIND_H

#include <set>
#include "mcrl2/core/detail/find.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace data {

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all variables occurring in t
  ///             are added.
  /// \return All variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void new_find_variables(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<variable, data::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All variables that occur in the term t
  template <typename Container>
  std::set<variable> new_find_variables(Container const& container)
  {
    std::set<variable> result;
    data::new_find_variables(container, std::inserter(result, result.end()));
    return result;
  }

  // TODO: this is a temporary solution
  struct compare_data_variable
  {
    const variable& m_variable;
  
    compare_data_variable(const variable& t)
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
  bool new_search_variable(Container const& container, const variable& v)
  {
    return core::detail::make_search_helper<variable, data::selective_traverser>(compare_data_variable(v)).apply(container);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NEW_FIND_H
