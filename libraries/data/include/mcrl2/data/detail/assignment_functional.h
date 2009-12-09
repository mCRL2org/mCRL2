// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_assignment_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_ASSIGNMENT_FUNCTIONAL_H
#define MCRL2_DATA_DETAIL_DATA_ASSIGNMENT_FUNCTIONAL_H

#include <set>
#include "mcrl2/data/variable.h"
#include "mcrl2/data/assignment.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// Returns the left hand side of a data assignment.
struct assignment_lhs: public std::unary_function<assignment, variable>
{
  /// \brief Function call operator
  /// \param a An assignment to a data variable
  /// \return The function result
  variable operator()(const assignment& a) const
  {
    return a.lhs();
  }
};

/// Returns true if the left hand side of an assignment equals a given variable.
struct has_left_hand_side
{
  const variable& m_variable;

  has_left_hand_side(const variable& variable)
   : m_variable(variable)
  {}

  /// \brief Function call operator
  /// \param a An assignment to a data variable
  /// \return The function result
  bool operator()(const assignment& a) const
  {
    return a.lhs() == m_variable;
  }
};

/// Returns true if the left hand side of an assignment is contained in a given set.
struct has_left_hand_side_in
{
  const std::set<variable>& m_variables;

  has_left_hand_side_in(const std::set<variable>& variables)
    : m_variables(variables)
  {}

  /// \brief Function call operator
  /// \param a An assignment to a data variable
  /// \return The function result
  bool operator()(assignment a) const
  {
    return m_variables.find(a.lhs()) != m_variables.end();
  }
};

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_ASSIGNMENT_FUNCTIONAL_H
